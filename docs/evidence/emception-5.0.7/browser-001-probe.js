window.probe={phase:'import',cases:[],done:false};
try {
 const {createEmception}=await import('/packages/@gameguild/emception-browser/dist/createEmception.js');
 window.probe.phase='boot';
 const em=await createEmception({tty:'none',manifestUrl:'/cdn/manifest.json'});
 window.probe.isolation=crossOriginIsolated;
 const cases=[
 ['version',null,['--version']],
 ['cpp','#include <vector>\n#include <iostream>\nint main(){std::vector<int> v{1,2,3};std::cout<<v.size()<<std::endl;}',['/home/user/cpp.cpp','-std=c++17','-O0','-sERROR_ON_UNDEFINED_SYMBOLS=1','-o','/home/user/cpp.js']],
 ['pthread','#include <pthread.h>\nvoid* f(void*){return nullptr;}\nint main(){pthread_t t;return pthread_create(&t,nullptr,f,nullptr);}',['/home/user/pthread.cpp','-pthread','-sPTHREAD_POOL_SIZE=1','-sABORTING_MALLOC=0','-sERROR_ON_UNDEFINED_SYMBOLS=1','-o','/home/user/pthread.js']],
 ['webgl2','#include <emscripten/html5.h>\n#include <GLES3/gl3.h>\nint main(){EmscriptenWebGLContextAttributes a;emscripten_webgl_init_context_attributes(&a);a.majorVersion=2;auto c=emscripten_webgl_create_context("#canvas",&a);emscripten_webgl_make_context_current(c);glClear(GL_COLOR_BUFFER_BIT);return !c;}',['/home/user/webgl2.cpp','-pthread','-sMIN_WEBGL_VERSION=2','-sMAX_WEBGL_VERSION=2','-sFULL_ES3=0','-sOFFSCREENCANVAS_SUPPORT=1','-sABORTING_MALLOC=0','-sERROR_ON_UNDEFINED_SYMBOLS=1','-o','/home/user/webgl2.js']]
 ];
 for(const [name,source,args] of cases){
  window.probe.phase=name;
  if(source)await em.workspace.writeFile('/home/user/'+name+'.cpp',source);
  const result=await em.run('em++',args);
  window.probe.cases.push({name,source,args,result});
 }
 em.dispose();window.probe.done=true;
} catch(e){window.probe.error=String(e);window.probe.stack=e.stack;window.probe.done=true;}
