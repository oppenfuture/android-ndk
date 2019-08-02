/*
 * Copyright (C) 2007 The Android Open Source Project
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

package com.android.gl2jni;

import android.app.Activity;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.util.Log;
import android.view.WindowManager;

import java.io.File;
import android.content.Context;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileOutputStream;


public class GL2JNIActivity extends Activity {

    GL2JNIView mView;

    @Override protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        copyAssets();
        mView = new GL2JNIView(getApplication());
	    setContentView(mView);
    }

    @Override protected void onPause() {
        super.onPause();
        mView.onPause();
    }

    @Override protected void onResume() {
        super.onResume();
        mView.onResume();
    }

    private void copyAssets() {
        AssetManager assetManager = getResources().getAssets();
        String[] files = null;

        try {
            files = assetManager.list("Files");
        } catch (Exception e) {
            Log.d("GL2Java", "ERROR : " + e.toString());
        }

        for (int i = 0; i < files.length; i++) {
            InputStream in = null;
            OutputStream out = null;
            FileOutputStream fileOutStream = null;
            try {
                Log.d("GL2Java", "file names : " + files[i]);

                in = assetManager.open("Files/" + files[i]);
                out = new FileOutputStream(getApplicationContext().getFilesDir() + files[i]);

                File file = new File(getApplicationContext().getFilesDir(), files[i]);

                byte[] buffer = new byte[65536 * 2];
                int read;
                while ((read = in.read(buffer)) != -1) {
                    out.write(buffer, 0, read);
                }
                in.close();
                in = null;

                out.flush();
                fileOutStream = new FileOutputStream(file);
                fileOutStream.write(buffer);
                out.close();
                out = null;
                Log.d("GL2Java", "File Copied in storage");
            } catch (Exception e) {
                Log.d("GL2Java", "ERROR: " + e.toString());
            }
        }
    }

}
