/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// OpenGL ES 2.0 code

#include <jni.h>
#include <android/log.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <FrostFire/irrlicht.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>

#include <string>
#include <gli/gli.hpp>

#define  LOG_TAG    "libgl2jni"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}

static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error
                                                    = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

std::string gTextureFilename;

auto vs1 = R"(precision highp float;
attribute vec3 inVertexPosition;
uniform mat4 _MVP;
void main(){
  gl_Position = _MVP * vec4(inVertexPosition, 1.0);
}
)";

auto fs1 = R"(precision highp float;
uniform sampler2D albedo;
uniform vec4 viewport_size[2];
void main()
{
  vec4 color = vec4(0.7,0.8,0.9,1.0);
  color = texture2D(albedo, gl_FragCoord.xy / vec2(viewport_size[1].z, viewport_size[1].w));
  gl_FragColor = color;
}
)";

auto config1 = R"({
  "materialrenderers": [
    {
      "name": "pass1",
      "pass": [
        {
          "zwrite": true,
          "ztest": "lessequal",
          "programid": 0
        }
      ]
    }
  ],
  "programs": [
    {
      "vshsrc": "{{vs}}",
      "fshsrc": "{{fs}}"
    }
  ]
})";

auto vs2 = R"(precision highp float;
attribute vec3 inVertexPosition;
varying vec2 TexCoord;
void main() {
    TexCoord = inVertexPosition.xy;
    gl_Position = vec4(inVertexPosition, 1.0);
})";

auto fs2 = R"(precision highp float;
uniform sampler2D src;
varying vec2 TexCoord;
void main() {
    gl_FragColor = texture2D(src, TexCoord);
})";

auto config2 = R"({
  "materialrenderers": [
    {
      "name": "pass2",
      "pass": [
        {
          "zwrite": true,
          "ztest": "lessequal",
          "programid": 0
        }
      ]
    }
  ],
  "programs": [
    {
      "vshsrc": "{{vs}}",
      "fshsrc": "{{fs}}"
    }
  ]
})";

std::string genConfig(std::string config, const std::string &vs, const std::string &fs) {
    config.replace(config.find("{{vs}}"), 6, vs);
    config.replace(config.find("{{fs}}"), 6, fs);
    return std::move(config);
}

irr::video::IVideoDriver *driver;
irr::scene::ISceneManager *scene_mgr;
gli::texture texture;
gli::gl::format texture_format;
irr::video::ITexture *irr_texture;
irr::video::ITexture *rt;
irr::scene::ISceneNode *node1, *node2;

bool setupGraphics(int w, int h) {
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);

    LOGI("setupGraphics(%d, %d)", w, h);

    irr::core::dimension2d<irr::u32> size((irr::u32)w, (irr::u32)h);
    auto device = irr::createDevice(irr::video::EDT_OGLES2, {(irr::u32)w, (irr::u32)h}, 16, false, false, false);
    if (!device) {
        LOGE("Could not create irr device.");
        return false;
    }
    driver = device->getVideoDriver();
    rt = nullptr;
    if (driver->queryFeature(irr::video::EVDF_RENDER_TO_TARGET)) {
        rt = driver->addRenderTargetTexture({w, h}, "rt", irr::video::ECF_R8G8B8);
        if (rt) {
            LOGI("Render target created");
        } else {
            LOGE("Failed to create render target");
        }
    } else {
        LOGE("No support for render target");
    }

    scene_mgr = device->getSceneManager();

    auto pass1_config = genConfig(config1, vs1, fs1);
    driver->CreateCustomMaterialRenderers(*pass1_config.c_str());
    std::vector<float> vertices1 = {-0.5f, 0.5f, 0.6f, 0.5f, 0.5f, 0.6f, -0.5f, -0.5f, 0.6f, 0.5f, -0.5f, 0.6f, 0.f, -1.f ,0.6f};
    std::vector<uint32_t> indices1 = {0,2,3,3,1,0,2,4,3};
    node1 = scene_mgr->addCustomMeshSceneNode(&driver->getMaterialRenderer("pass1")->InitMaterial,
            vertices1.data(), (uint32_t)vertices1.size(), indices1.data(), (uint32_t)indices1.size());

    auto pass2_config = genConfig(config2, vs2, fs2);
    driver->CreateCustomMaterialRenderers(*pass2_config.c_str());
    std::vector<float> vertices2 = {0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f, 1.f, 1.f};
    std::vector<uint32_t> indices2 = {0, 1, 2, 0, 2, 3};
    node2 = scene_mgr->addCustomMeshSceneNode(&driver->getMaterialRenderer("pass2")->InitMaterial,
            vertices2.data(), (uint32_t)vertices2.size(), indices2.data(), (uint32_t)indices2.size());

    texture = gli::load(gTextureFilename);
    gli::gl gli_gl(gli::gl::PROFILE_ES20);
    texture_format = gli_gl.translate(texture.format(), texture.swizzles());
    auto data_size = texture.size(0);
    auto pixels = new uint8_t[data_size];
    memset(pixels, 0, data_size);
    irr_texture = driver->createTexture(GL_TEXTURE_2D, (irr::u32)texture.extent(0).x, (irr::u32)texture.extent(0).y, data_size, texture_format.Internal, pixels, 0);
    delete[] pixels;
    node1->getMaterial(0).getPass(0).setTexture("albedo", irr_texture);
    float viewport_size[8] = {0, 0, 0, 0, 0, 0, (float)w, (float)h};
    node1->getMaterial(0).getPass(0).setVector("viewport_size", viewport_size, 8);

    if(rt) node2->getMaterial(0).getPass(0).setTexture("src", rt);

    scene_mgr->addCameraSceneNode(nullptr, irr::core::vector3df(0, 1, 2.5), irr::core::vector3df(0, 0, 0));
    return true;
}

void renderFrame() {
    static float grey;
    grey += 0.01f;
    if (grey > 1.0f) {
        grey = 0.0f;
    }

    irr_texture->loadRawSubTexture(GL_TEXTURE_2D, 0, 0, (irr::u32)texture.extent(0).x, (irr::u32)texture.extent(0).y, texture.size(0), texture_format.Internal, texture.data(), 0);

    auto clear_flag = irr::video::ECBF_COLOR | irr::video:: ECBF_DEPTH;
    irr::u32 int_grey = (irr::u32)(grey * 255);
    auto clear_color1 = irr::video::SColor(255, int_grey, int_grey, int_grey);
    auto clear_color2 = irr::video::SColor(255, 255 - int_grey, 255 - int_grey, 255 - int_grey);
    driver->beginScene(clear_flag);

    if (rt) {
        if (!driver->setRenderTarget(rt, clear_flag, clear_color1)) {
            LOGE("Set render target to rt failed");
        }
        node1->setVisible(true);
        node2->setVisible(false);
    }

    scene_mgr->drawAll();

    if (rt) {
        if (!driver->setRenderTarget(nullptr, clear_flag, clear_color2)) {
            LOGE("Set render target to null failed");
        }
        node1->setVisible(false);
        node2->setVisible(true);
        scene_mgr->drawAll();
    }

    driver->endScene();
}

extern "C" {
JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj,  jint width, jint height);
JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jobject obj);
JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_setTexturePath(JNIEnv * env, jobject obj, jstring str);
};

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj,  jint width, jint height)
{
    setupGraphics(width, height);
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jobject obj)
{
    renderFrame();
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_setTexturePath(JNIEnv * env, jobject obj, jstring str) {
    const jsize len = env->GetStringUTFLength(str);
    const char *strChars = env->GetStringUTFChars(str, (jboolean *)0);
    gTextureFilename = std::string(strChars, len);
    env->ReleaseStringUTFChars(str, strChars);
}
