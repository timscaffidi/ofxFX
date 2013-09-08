/*
 *  ofxBlurFast.h
 *
 *  Created by Patricio Gonzalez Vivo on 25/11/11.
 *  Copyright (c) 2011 http://PatricioGonzalezVivo.com All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the author nor the names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 *  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 *  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 *  OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  ************************************************************************************ 
 *  
 *  This use a convolution matrix to make the blur so you can actually modify this same
 *  addon to do different things.
 *
 *  Take a look at:
 *
 *     http://www.jasonwaltman.com/thesis/filter-convolution.html
 *
 *  You can alter the fade value to:
 *
 *   1.0 -> unsharp the edges ( make it more smooth )
 *   0.0
 *  -1.0 -> sharp the edges
 *
 */

#ifndef OFXUNSHARP
#define OFXUNSHARP

#include "ofMain.h"
#include "ofxFXObject.h"

class ofxBlur : public ofxFXObject {
public:
    ofxBlur(){
        passes = 3;
        internalFormat = GL_RGBA;
        
        // Fade constant
        fade = 0.03f;
        
        // In this example the tex0 it's more like a backbuffer 
        fragmentShader = STRINGIFY(
                                   uniform sampler2DRect tex0;
                                   uniform float fade;
                                   
                                   float kernel[9];
                                   vec2 offset[9];
                                   
                                   void main(void){
                                       vec2  st = gl_TexCoord[0].st;
                                       vec4 sum = vec4(0.0);
                                       
                                       offset[0] = vec2(-1.0, -1.0);
                                       offset[1] = vec2(0.0, -1.0);
                                       offset[2] = vec2(1.0, -1.0);
                                       
                                       offset[3] = vec2(-1.0, 0.0);
                                       offset[4] = vec2(0.0, 0.0);
                                       offset[5] = vec2(1.0, 0.0);
                                       
                                       offset[6] = vec2(-1.0, 1.0);
                                       offset[7] = vec2(0.0, 1.0);
                                       offset[8] = vec2(1.0, 1.0);
                                       
                                       kernel[0] = 1.0/16.0;   kernel[1] = 2.0/16.0;   kernel[2] = 1.0/16.0;
                                       kernel[3] = 2.0/16.0;   kernel[4] = 4.0/16.0;   kernel[5] = 2.0/16.0;
                                       kernel[6] = 1.0/16.0;   kernel[7] = 2.0/16.0;   kernel[8] = 1.0/16.0;
                                       
                                       int i = 0;
                                       for (i = 0; i < 9; i++){
                                           vec4 tmp = texture2DRect(tex0, st + offset[i]);
                                           sum += tmp * kernel[i];
                                       }
                                       
                                       vec4 previousValue = texture2DRect(tex0, st);
                                       gl_FragColor = (1.0 - fade) * previousValue +  fade * vec4(sum.rgb, previousValue.a);
                                   }
                                   );
         
    };
    
	void setFade(float _fade) { fade = _fade;};
    
	void update(){
        ofPushStyle();
        
        ofSetColor(255);
        pingPong.src->begin();
        ofClear(0,0);
        textures[0].draw(0,0);
        pingPong.src->end();
        
        for(int i = 0; i < passes; i++) {
            pingPong.dst->begin();
            ofClear(0,0);
            shader.begin();
            // In this example the tex0 it's more like a backbuffer 
            shader.setUniformTexture("tex0", pingPong.src->getTextureReference(), 0 );
            shader.setUniform1f("fade", fade);
            renderFrame();
            shader.end();
            pingPong.dst->end();
            
            pingPong.swap();
        }
        
        pingPong.swap();
        
        ofPopStyle();
	}
	
protected:
	float       fade;
};
#endif
