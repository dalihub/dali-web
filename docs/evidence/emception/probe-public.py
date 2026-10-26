import json,os,socket,subprocess,time,urllib.request
from pathlib import Path
logs=Path('${DALI_WEB_ROOT}/logs/emception/assessment-001')
profiles=logs/'profiles';profiles.mkdir(exist_ok=True)
with socket.socket() as s:s.bind(('127.0.0.1',0));port=s.getsockname()[1]
endpoint=f'http://127.0.0.1:{port}'
def req(method,path,data=None):
 q=urllib.request.Request(endpoint+path,data=None if data is None else json.dumps(data).encode(),method=method,headers={'Content-Type':'application/json'})
 with urllib.request.urlopen(q,timeout=40) as r:return json.load(r)['value']
sid=None;results=[]
with (logs/'geckodriver.log').open('w') as log:
 driver=subprocess.Popen(['geckodriver','--port',str(port),'--profile-root',str(profiles)],stdout=log,stderr=log)
 try:
  for _ in range(100):
   try:req('GET','/status');break
   except OSError:
    if driver.poll() is not None:raise RuntimeError('geckodriver exited')
    time.sleep(.1)
  session=req('POST','/session',{'capabilities':{'alwaysMatch':{'browserName':'firefox','moz:firefoxOptions':{'args':['-headless']}}}})
  sid=session['sessionId'];base='/session/'+sid
  (logs/'browser-capabilities.json').write_text(json.dumps(session,indent=2))
  req('POST',base+'/timeouts',{'pageLoad':30000,'script':30000})
  req('POST',base+'/url',{'url':'https://jprendes.github.io/emception/'})
  def js(script,args=[]):return req('POST',base+'/execute/sync',{'script':script,'args':args})
  t=time.monotonic()
  for i in range(240):
   state=js('return {ready:!!window.emception && !!document.querySelector("#compile") && !document.querySelector("#compile").disabled,text:document.body.innerText,isolation:crossOriginIsolated};')
   if i%15==0:print('loading',round(time.monotonic()-t),str(state)[:250],flush=True)
   if state['ready']:break
   time.sleep(1)
  (logs/'browser-ready.json').write_text(json.dumps(state,indent=2))
  if not state['ready']:raise RuntimeError('initialization timeout')
  cases=[('version',None,['em++','--version']),('hello','#include <iostream>\n#include <vector>\nint main(){std::vector<int> v{1,2,3};std::cout << v.size();}\n',['em++','hello.cpp','-std=c++17','-O0','-sSINGLE_FILE=1','-o','hello.js']),('syntax-error','int main( {',['em++','syntax-error.cpp','-o','bad.js']),('pthread','#include <pthread.h>\nvoid* work(void*){return 0;}\nint main(){pthread_t t;return pthread_create(&t,0,work,0);}\n',['em++','pthread.cpp','-pthread','-sPTHREAD_POOL_SIZE=1','-sABORTING_MALLOC=0','-o','pthread.js']),('webgl2','#include <emscripten/html5.h>\n#include <GLES3/gl3.h>\nint main(){EmscriptenWebGLContextAttributes a;emscripten_webgl_init_context_attributes(&a);a.majorVersion=2;auto c=emscripten_webgl_create_context("#canvas",&a);emscripten_webgl_make_context_current(c);glClear(GL_COLOR_BUFFER_BIT);return !c;}\n',['em++','webgl2.cpp','-pthread','-sMIN_WEBGL_VERSION=2','-sMAX_WEBGL_VERSION=2','-sFULL_ES3=0','-sOFFSCREENCANVAS_SUPPORT=1','-sABORTING_MALLOC=0','-sERROR_ON_UNDEFINED_SYMBOLS=1','-o','webgl2.js'])]
  for name,source,argv in cases:
   js('window.checkResult=null;(async()=>{const t=performance.now();try{if(arguments[1]!==null) await emception.fileSystem.writeFile("/working/"+arguments[0]+".cpp",arguments[1]);const r=await emception.run(...arguments[2]);window.checkResult={...r,ms:performance.now()-t};}catch(e){window.checkResult={error:String(e),ms:performance.now()-t};}})();',[name,source,argv])
   for _ in range(180):
    result=js('return window.checkResult;')
    if result is not None:break
    time.sleep(1)
   item={'name':name,'source':source,'argv':argv,'result':result};results.append(item)
   (logs/'browser-tests.json').write_text(json.dumps(results,indent=2));print(json.dumps(item),flush=True)
   if result is None:break
  resources=js('return performance.getEntriesByType("resource").map(x=>({name:x.name,bytes:x.transferSize,duration:x.duration}));')
  (logs/'resources.json').write_text(json.dumps(resources,indent=2))
 finally:
  if sid:
   try:req('DELETE','/session/'+sid)
   except Exception:pass
  driver.terminate()
  try:driver.wait(timeout=10)
  except subprocess.TimeoutExpired:driver.kill();driver.wait()
