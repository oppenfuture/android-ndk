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
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

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


auto vs = R"(
precision highp float;
attribute vec3 inVertexPosition;
uniform mat4 _MVP;
void main(void) {
  gl_Position = _MVP * vec4(inVertexPosition, 1.0);
}
)";

auto fs = R"(
precision highp float;
void main(void) {
  vec4 color = vec4(1, 0, 0, 1.0) ;
  gl_FragColor = color;
}
)";


auto fs_transparent = R"(
precision highp float;
void main(void) {
  vec4 color = vec4(0.0, 1.0, 0.0, 0.5);
  gl_FragColor = color ;
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
          "zwrite": false,
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

auto config_transparent = R"({
  "materialrenderers": [
    {
      "name": "renderer_transparent",
      "pass": [
        {
          "queue": 3000,
          "blendenalble": true,
          "blendsrc": "srcalpha",
          "blenddst": "oneminussrcalpha",
          "zwrite": false,
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

    //配置非透明物体
    auto renderer_config = genConfig(config, vs, fs);
    driver->CreateCustomMaterialRenderers(*renderer_config.c_str());
    auto renderer = driver->getMaterialRenderer("renderer");
    float vertices[] = {-1.f, -1.f, -4.0f, 1.f,  -1.f, -4.0f,
                        1.f,  1.f,  -4.0f, -1.f, 1.f,  -4.0f};
    uint32_t indices[] = {0, 1, 2, 0, 2, 3};
    //配置透明物体
    auto renderer_config_transparent = genConfig(config_transparent, vs, fs_transparent);
    driver->CreateCustomMaterialRenderers(*renderer_config_transparent.c_str());
    auto renderer_transparent = driver->getMaterialRenderer("renderer_transparent");
    float vertices_transparent[] = {0.f, -1.f, -3.0f, 2.f,  -1.f, -3.0f,
                        2.f,  1.f,  -3.0f, 0.f, 1.f,  -3.0f};

    //添加mesh
    scene_mgr->addCustomMeshSceneNode(
            &renderer->InitMaterial, vertices, 4, indices, 6);
    scene_mgr->addCustomMeshSceneNode(
            &renderer_transparent->InitMaterial, vertices_transparent, 4, indices, 6);
    
    //放置相机
    irr::scene::ICameraSceneNode *camera_noe = scene_mgr->addCameraSceneNode();
    irr::core::vector3df target(0, 0, -1);
    irr::core::vector3df origin(0, 0, 2);
    camera_noe->setTarget(target);
    camera_noe->setPosition(origin);
    return true;
}

void renderFrame() {
    auto clear_flag = irr::video::ECBF_COLOR | irr::video:: ECBF_DEPTH;
    driver->beginScene(clear_flag);
    scene_mgr->drawAll();
    driver->endScene();
}

extern "C" {
JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj,  jint width, jint height);
JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jobject obj);
};

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj,  jint width, jint height)
{
    setupGraphics(width, height);
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jobject obj)
{
    renderFrame();
}








