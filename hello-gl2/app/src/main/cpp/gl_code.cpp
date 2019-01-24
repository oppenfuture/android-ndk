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

auto vs = R"(precision highp float;
attribute vec3 inVertexPosition;
attribute vec3 inVertexNormal;
attribute vec2 inTexCoord0;
uniform mat4 _MVP;
uniform vec4 albedo_uv;
varying vec2 v_UV0;
varying vec3 v_vPos;
void main(){
  gl_Position = _MVP * vec4(inVertexPosition, 1.0);
  v_UV0 =inTexCoord0;
}
)";

auto fs = R"(precision highp float;
uniform float _Time;
uniform int albedousage;
uniform sampler2D albedo;
varying vec2 v_UV0;
void main()
{
  vec4 color = vec4(0.7,0.8,0.9,1.0);
  if (bool(albedousage)) {
    color = texture2D(albedo, gl_FragCoord.xy / vec2(1000.0, 1000.0));
  }
  gl_FragColor = color;
}
)";

auto customRender = R"({
  "materialrenderers": [
    {
      "name": "customShader",
      "pass": [
        {
          "queue": 2000,
          "blendenalble": true,
          "blendsrc": "one",
          "blenddst": "one",
          "zwrite": true,
          "ztest": "always",
          "cull": "back",
          "colormask": {
            "r": true,
            "g": true,
            "b": true,
            "a": true
          },
          "programid": 0
        }
      ]
    }
  ],
  "programs": [
    {
      "vshsrc": "{{vs}}",
      "fshsrc": "{{fs}}",
      "uniforms": [
        {
          "name": "albedo_uv",
          "type": "vec4",
          "defaultvalue": {
            "r": 1,
            "g": 1,
            "b": 0,
            "a": 0
          }
        },
        {
          "name": "albedo",
          "type": "sampler2D"
        },
        {
          "name": "albedousage",
          "semantic": "albedo"
        },
        {
          "name": "_Time",
          "semantic": "_Time",
          "type": "float"
        },
        {
          "name": "_MVP",
          "semantic": "_MVP"
        }
      ],
      "attributes": [
        {
          "name": "inVertexPosition",
          "semantic": "inVertexPosition"
        },
        {
          "name": "inVertexNormal",
          "semantic": "inVertexNormal"
        },
        {
          "name": "inTexCoord0",
          "semantic": "inTexCoord0"
        }
      ]
    }
  ]
})";

irr::video::IVideoDriver *driver;
irr::scene::ISceneManager *scene_mgr;
gli::texture texture;
gli::gl::format texture_format;
irr::video::ITexture *irr_texture;

bool setupGraphics(int w, int h) {
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);

    LOGI("setupGraphics(%d, %d)", w, h);

    auto device = irr::createDevice(irr::video::EDT_OGLES2, {w, h}, 16, false, false, false);
    if (!device) {
        LOGE("Could not create irr device.");
        return false;
    }
    driver = device->getVideoDriver();
    scene_mgr = device->getSceneManager();

    std::string render_config = customRender;
    render_config.replace(render_config.find("{{vs}}"), 6, vs);
    render_config.replace(render_config.find("{{fs}}"), 6, fs);

    driver->CreateCustomMaterialRenderers(*render_config.c_str());

    auto material = driver->getMaterialRenderer("customShader")->InitMaterial;
    material.getPass(0).BlendOperation = irr::video::EBO_ADD;

    float vertices[] = {-0.5f, 0.5f, 0.6f, 0.5f, 0.5f, 0.6f, -0.5f, -0.5f, 0.6f, 0.5f, -0.5f, 0.6f, 0.f, -1.f ,0.6f};
    uint32_t indices[] = {0,2,3,3,1,0,2,4,3};
    auto node = scene_mgr->addCustomMeshSceneNode(&material, vertices, 5, indices, 9);

    texture = gli::load(gTextureFilename);
    gli::gl gli_gl(gli::gl::PROFILE_ES20);
    texture_format = gli_gl.translate(texture.format(), texture.swizzles());
    auto data_size = texture.size(0);
    auto pixels = new uint8_t[data_size];
    memset(pixels, 0, data_size);
    irr_texture = driver->createTexture(GL_TEXTURE_2D, (irr::u32)texture.extent(0).x, (irr::u32)texture.extent(0).y, data_size, texture_format.Internal, pixels, 0);
    delete[] pixels;
    node->getMaterial(0).getPass(0).setTexture("albedo", irr_texture);

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

    irr::u32 int_grey = (irr::u32)(grey * 255);
    driver->beginScene(irr::video::ECBF_COLOR | irr::video:: ECBF_DEPTH, irr::video::SColor(255, int_grey, int_grey, int_grey));
    scene_mgr->drawAll();
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
