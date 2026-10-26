window.probe={phase:'boot',cases:[],done:false,logs:[]};
for(const name of ['log','warn','error']){const orig=console[name];console[name]=(...args)=>{window.probe.logs.push({level:name,text:args.map(String).join(' ')});if(window.probe.logs.length>120)window.probe.logs.shift();orig(...args);};}
try {
 const {createEmception}=await import('/packages/@gameguild/emception-browser/dist/createEmception.js');
 const em=await createEmception({tty:'none',manifestUrl:'/cdn/manifest.json'});
 const source='#include <emscripten/html5.h>\n#include <GLES3/gl3.h>\nint main(){EmscriptenWebGLContextAttributes a;emscripten_webgl_init_context_attributes(&a);a.majorVersion=2;auto c=emscripten_webgl_create_context("#canvas",&a);emscripten_webgl_make_context_current(c);glClear(GL_COLOR_BUFFER_BIT);return 0;}';
 await em.workspace.writeFile('/home/user/minimal.cpp',source);
 window.probe.phase='em++-minimal';
 const args=['em++','/home/user/minimal.cpp','-O2','-sUSE_WEBGL2=1','-sERROR_ON_UNDEFINED_SYMBOLS=1','-o','/home/user/minimal.js'];
 const result=await Promise.race([em.run('em++',args),new Promise(resolve=>setTimeout(()=>resolve({timedOut:true}),300000))]);
 const files={};
 if(!result.timedOut)for(const ext of ['js','wasm']){try{const bytes=await em.workspace.readFile('/home/user/minimal.'+ext);files[ext]={bytes:bytes.byteLength,head:Array.from(bytes.slice(0,8))};}catch(e){files[ext]={error:String(e)};}}
 window.probe.cases.push({name:'em++-minimal',source,args,result,files});
 em.dispose();window.probe.done=true;
}catch(e){window.probe.error=String(e);window.probe.done=true;}
