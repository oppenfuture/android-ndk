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

#include <frostfire/irrlicht.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>

#include <string>
#include <gli/gli.hpp>
#include "include/frostfire/ISceneNode.h"

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

auto vs = R"(
precision highp float;
attribute vec3 inVertexPosition;
varying vec2 texcoord;
void main(void) {
  texcoord = (inVertexPosition.xy + 1.0) / 2.0;
  gl_Position = vec4(inVertexPosition, 1.0);
}
)";

auto fs = R"(
precision highp float;
uniform sampler2D texture1;
uniform sampler2D texture2;
varying vec2 texcoord;
void main(void) {
  vec4 color = vec4(0.8, 0.9, 0.9, 1.0);
  if (texcoord.y < 0.4) {
    color = texture2D(texture1, vec2(texcoord.x, texcoord.y * 2.5));
  }

  if (texcoord.y > 0.6) {
    color = texture2D(texture2, vec2(texcoord.x, texcoord.y * 2.5 - 1.5));
  }

  gl_FragColor = color;
}
)";

auto config = R"({
  "materialrenderers": [
    {
      "name": "renderer",
      "pass": [
        {
          "queue": 2000,
          "blendenalble": false,
          "zwrite": true,
          "ztest": "lessequal",
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
          "name": "texture1",
          "type": "sampler2D"
        },
        {
          "name": "texture2",
          "type": "sampler2D"
        }
      ],
      "attributes": [
        {
          "name": "inVertexPosition",
          "semantic": "inVertexPosition"
        }
      ]
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
irr::scene::ISceneNode *screen_quad_node;

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
    scene_mgr = device->getSceneManager();

    auto renderer_config = genConfig(config, vs, fs);
    driver->CreateCustomMaterialRenderers(*renderer_config.c_str());
    auto renderer = driver->getMaterialRenderer("renderer");
    float vertices[] = {-1.f, -1.f, 1.0f, 1.f,  -1.f, 1.0f,
                        1.f,  1.f,  1.0f, -1.f, 1.f,  1.0f};
    uint32_t indices[] = {0, 1, 2, 0, 2, 3};
    screen_quad_node = scene_mgr->addCustomMeshSceneNode(
            &renderer->InitMaterial, vertices, 4, indices, 6);

    texture = gli::load(gTextureFilename);
    gli::gl gli_gl(gli::gl::PROFILE_ES20);
    texture_format = gli_gl.translate(texture.format(), texture.swizzles());
    auto data_size = texture.size(0);
    auto pixels = new uint8_t[data_size];
    memset(pixels, 0, data_size);
    auto texture1 = driver->createTexture(GL_TEXTURE_2D, (irr::u32)texture.extent(0).x, (irr::u32)texture.extent(0).y, data_size, texture_format.Internal, pixels, 0);
    auto texture2 = driver->createTexture(GL_TEXTURE_2D, (irr::u32)texture.extent(0).x, (irr::u32)texture.extent(0).y, data_size, texture_format.Internal, pixels, 0);
    delete[] pixels;

    auto &pass = screen_quad_node->getMaterial(0).getPass(0);
    pass.setTexture("texture1", texture1);
    pass.setTexture("texture2", texture2);

    scene_mgr->addCameraSceneNode();
    return true;
}

void renderFrame() {
    static float grey;
    grey += 0.01f;
    if (grey > 1.0f) {
        grey = 0.0f;
    }

    screen_quad_node->getMaterial(0).getPass(0).updateSubTexture("texture1",
                                                                 0, 0, (irr::u32)texture.extent(0).x, (irr::u32)texture.extent(0).y,
                                                                 texture.size(0), texture_format.Internal, texture.data());

    auto clear_flag = irr::video::ECBF_COLOR | irr::video:: ECBF_DEPTH;
    driver->beginScene(clear_flag);
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
