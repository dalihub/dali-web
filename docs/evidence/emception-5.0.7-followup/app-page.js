"use client";
import {useEffect} from 'react';
import {createEmception} from '@gameguild/emception-browser';
export default function Page(){useEffect(()=>{if(window.probe)return;window.probe={cases:[],logs:[],done:false};
for(const name of ['log','warn','error']){const orig=console[name];console[name]=(...a)=>{window.probe.logs.push({level:name,text:a.map(String).join(' ')});if(window.probe.logs.length>250)window.probe.logs.shift();orig(...a);};}
(async()=>{let em;try{em=await createEmception({tty:'none',manifestUrl:'/cdn/manifest.json'});const mode=new URLSearchParams(location.search).get('mode')||'official';window.probe.phase=mode;
let task;
if(mode==='official')task=em.compileAndRun('#include <stdio.h>\nint main(){puts("HELLO_OFFICIAL");return 0;}',{stdout:'capture',stderr:'capture'});
else if(mode==='official-absolute'||mode==='official-stl'){await em.workspace.writeFile('/home/user/main.cpp',mode==='official-stl'?'#include <vector>\n#include <iostream>\nint main(){std::vector<int> v{1,2,3};std::cout<<"STL:"<<v.size()<<std::endl;return v.size()!=3;}':'#include <stdio.h>\nint main(){puts("HELLO_OFFICIAL_ABSOLUTE");return 0;}');task=em.compileAndRun(undefined,{sources:['/home/user/main.cpp'],stdout:'capture',stderr:'capture'});}
else {let source='int main(){return 0;}';let flags=['-O0'];
if(mode==='optimized')flags=['-O2'];
if(mode==='pthread'){source='#include <pthread.h>\nvoid* f(void*){return 0;} int main(){pthread_t t;pthread_create(&t,0,f,0);pthread_join(t,0);return 0;}';flags=['-O2','-pthread','-sPTHREAD_POOL_SIZE=1'];}
if(mode==='webgl'){source='#include <emscripten/html5.h>\n#include <GLES3/gl3.h>\nint main(){EmscriptenWebGLContextAttributes a;emscripten_webgl_init_context_attributes(&a);a.majorVersion=2;auto c=emscripten_webgl_create_context("#canvas",&a);emscripten_webgl_make_context_current(c);glClear(GL_COLOR_BUFFER_BIT);return 0;}';flags=['-O2','-sUSE_WEBGL2=1'];}
await em.workspace.writeFile('/home/user/main.cpp',source);const args=['em++','/home/user/main.cpp',...flags,'-sERROR_ON_UNDEFINED_SYMBOLS=1','-o','/home/user/main.js'];window.probe.args=args;window.probe.source=source;task=em.run('em++',args);}
const result=await Promise.race([task,new Promise(r=>setTimeout(()=>r({timedOut:true}),300000))]);window.probe.cases.push({mode,result});if(!result.timedOut&&!mode.startsWith('official')){window.probe.files={};for(const ext of ['js','wasm']){try{const data=await em.workspace.readFile('/home/user/main.'+ext);window.probe.files[ext]={bytes:data.byteLength,head:Array.from(data.slice(0,8)),text:ext==='js'?new TextDecoder().decode(data).slice(0,1000):undefined};}catch(e){window.probe.files[ext]={error:String(e)};}}}
}catch(e){window.probe.error=String(e);}finally{em?.dispose();window.probe.done=true;}})();},[]);return <p>Local Emception validation</p>;}
