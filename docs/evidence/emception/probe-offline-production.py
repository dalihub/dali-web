import json,os,socket,subprocess,time,urllib.request
from pathlib import Path
logs=Path('${DALI_WEB_ROOT}/logs/emception/assessment-006')
logs.mkdir(parents=True,exist_ok=False)
profiles=logs/'profiles';profiles.mkdir(exist_ok=True)
with socket.socket() as s:s.bind(('127.0.0.1',0));port=s.getsockname()[1]
endpoint=f'http://127.0.0.1:{port}'
def req(method,path,data=None):
 q=urllib.request.Request(endpoint+path,data=None if data is None else json.dumps(data).encode(),method=method,headers={'Content-Type':'application/json'})
 with urllib.request.urlopen(q,timeout=40) as r:return json.load(r)['value']
import http.server,threading,functools
assets=Path('${DALI_WEB_ROOT}/logs/emception/offline-assets')
class Handler(http.server.SimpleHTTPRequestHandler):
 def end_headers(self):
  self.send_header('Cross-Origin-Opener-Policy','same-origin')
  self.send_header('Cross-Origin-Embedder-Policy','require-corp')
  self.send_header('Content-Security-Policy',"default-src 'none'; script-src 'self' 'unsafe-eval' blob:; connect-src 'self'; worker-src 'self' blob:; img-src 'none'; frame-src 'self'; form-action 'none'")
  super().end_headers()
 def log_message(self,fmt,*args):
  with (logs/'http.log').open('a') as f:f.write(fmt%args+'\n')
server=http.server.ThreadingHTTPServer(('127.0.0.1',0),functools.partial(Handler,directory=str(assets)))
threading.Thread(target=server.serve_forever,daemon=True).start()
sid=None;results=[]
with (logs/'geckodriver.log').open('w') as log:
 driver=subprocess.Popen(['geckodriver','--port',str(port),'--profile-root',str(profiles)],stdout=log,stderr=log)
 try:
  for _ in range(100):
   try:req('GET','/status');break
   except OSError:
    if driver.poll() is not None:raise RuntimeError('geckodriver exited')
    time.sleep(.1)
  session=req('POST','/session',{'capabilities':{'alwaysMatch':{'browserName':'firefox','moz:firefoxOptions':{'args':['-headless'],'prefs':{'network.proxy.type':1,'network.proxy.http':'127.0.0.1','network.proxy.http_port':9,'network.proxy.ssl':'127.0.0.1','network.proxy.ssl_port':9,'network.proxy.no_proxies_on':'localhost,127.0.0.1','network.proxy.failover_direct':False}}}}})
  sid=session['sessionId'];base='/session/'+sid
  (logs/'browser-capabilities.json').write_text(json.dumps(session,indent=2))
  req('POST',base+'/timeouts',{'pageLoad':30000,'script':30000})
  req('POST',base+'/url',{'url':f'http://127.0.0.1:{server.server_port}/index.html'})
  def js(script,args=[]):return req('POST',base+'/execute/sync',{'script':script,'args':args})
  t=time.monotonic()
  for i in range(240):
   state=js('return {ready:!!window.emception && !!document.querySelector("#compile") && !document.querySelector("#compile").disabled,text:document.body.innerText,isolation:crossOriginIsolated};')
   if i%15==0:print('loading',round(time.monotonic()-t),str(state)[:250],flush=True)
   if state['ready']:break
   time.sleep(1)
  (logs/'browser-ready.json').write_text(json.dumps(state,indent=2))
  if not state['ready']:raise RuntimeError('initialization timeout')
  root=Path('${DALI_WEB_ROOT}')
  includes={}
  for name in ['common/dali-common.h','common/type-traits.h','math/vector2.h','math/int-pair.h']:
   includes['dali/public-api/'+name]=(root/'libs/dali-core/dali/public-api'/name).read_text()
  includes['dali/public-api/common/dali-namespace.h']=(root/'build/p2-b/production-003/generated/include/dali/public-api/common/dali-namespace.h').read_text()
  js('window.filesReady=null;(async()=>{try{for(const [p,c] of Object.entries(arguments[0])){const f="/working/include/"+p;await emception.fileSystem.mkdirTree(f.slice(0,f.lastIndexOf("/")));await emception.fileSystem.writeFile(f,c);}window.filesReady=true;}catch(e){window.filesReady=String(e);}})();',[includes])
  for _ in range(30):
   if js('return window.filesReady;') is not None:break
   time.sleep(.1)
  import base64,hashlib
  objects={'vector2.o':root/'build/p2-b/production-003/core/CMakeFiles/dali2-core.dir${DALI_WEB_ROOT}/libs/dali-core/dali/public-api/math/vector2.cpp.o','host.o':root/'build/p2-b/production-003/CMakeFiles/dali-web.dir${DALI_WEB_ROOT}/web/host/web-host.cpp.o'}
  manifest={}
  for name,p in objects.items():
   data=p.read_bytes();manifest[name]={'path':str(p),'sha256':hashlib.sha256(data).hexdigest(),'bytes':len(data)}
   js('window.filesReady=null;emception.fileSystem.writeFile("/working/"+arguments[0],Uint8Array.from(atob(arguments[1]),c=>c.charCodeAt(0))).then(()=>window.filesReady=true).catch(e=>window.filesReady=String(e));',[name,base64.b64encode(data).decode()])
   for _ in range(30):
    if js('return window.filesReady;') is not None:break
    time.sleep(.1)
  (logs/'input-objects.json').write_text(json.dumps(manifest,indent=2))
  js('window.productionReady=null;(async()=>{try{await emception.fileSystem.mkdirTree("/working/production");for(const name of arguments[0]){const r=await fetch("production/"+name);if(!r.ok)throw new Error(name);const bytes=new Uint8Array(await r.arrayBuffer());await emception.fileSystem.writeFile("/working/production/"+name,bytes);}window.productionReady=true;}catch(e){window.productionReady=String(e);}})();',[['shader-parser.cpp.o', 'gles-graphics-program.cpp.o', 'gles-graphics-controller.cpp.o', 'gles-graphics-controller-debug.cpp.o', 'gles-graphics-buffer.cpp.o', 'gles-graphics-command-buffer.cpp.o', 'gles-graphics-debug.cpp.o', 'gles-graphics-framebuffer.cpp.o', 'gles-graphics-pipeline.cpp.o', 'gles-graphics-reflection.cpp.o', 'gles-graphics-render-pass.cpp.o', 'gles-graphics-render-target.cpp.o', 'gles-graphics-sampler.cpp.o', 'gles-graphics-shader.cpp.o', 'gles-graphics-texture.cpp.o', 'gles-graphics-pipeline-cache.cpp.o', 'gles-context.cpp.o', 'gles-sync-object.cpp.o', 'gles-framebuffer-state-cache.cpp.o', 'gles-sync-pool.cpp.o', 'gles-texture-dependency-checker.cpp.o', 'web-gl-implementation.cpp.o', 'web-gles-context.cpp.o', 'web-gles-rendering.cpp.o', 'web-gles-sync.cpp.o', 'web-gles-sync-pool.cpp.o', 'web-gles-fence.cpp.o', 'web-gles-memory.cpp.o', 'web-gles-buffer.cpp.o', 'web-buffer-memory.cpp.o', 'web-buffer-storage.cpp.o', 'web-gles-failure.cpp.o', 'web-gles-program-cache.cpp.o', 'environment-variable.cpp.o', 'web-host.cpp.o', 'libdali2-core.a']])
  for _ in range(120):
   ready=js('return window.productionReady;')
   if ready is not None:break
   time.sleep(.2)
  if ready is not True:raise RuntimeError(str(ready))
  cases=[('hello','#include <iostream>\n#include <vector>\nint main(){std::vector<int> v{1,2,3};std::cout << "RESULT:" << v.size() << std::endl;}\n',['em++','hello.cpp','-std=c++17','-O0','-sSINGLE_FILE=1','-sEXIT_RUNTIME=1','-o','hello.js']),('dali-vector','#include <dali/public-api/math/vector2.h>\n#include <cstdio>\nint main(){Dali::Vector2 v(3,4);printf("LENGTH:%.0f\\n",v.Length());return v.Length()==5?0:1;}\n',['em++','dali-vector.cpp','vector2.o','-Iinclude','-std=c++17','-O0','-sSINGLE_FILE=1','-sEXIT_RUNTIME=1','-sERROR_ON_UNDEFINED_SYMBOLS=1','-o','dali-vector.js']),('host-reloc',None,['/usr/bin/wasm-ld','-r','/working/host.o','-o','/working/host-reloc.o'])]
  cases.append(('production-link',None,['em++', '-pthread', '--no-entry', '/working/production/shader-parser.cpp.o', '/working/production/gles-graphics-program.cpp.o', '/working/production/gles-graphics-controller.cpp.o', '/working/production/gles-graphics-controller-debug.cpp.o', '/working/production/gles-graphics-buffer.cpp.o', '/working/production/gles-graphics-command-buffer.cpp.o', '/working/production/gles-graphics-debug.cpp.o', '/working/production/gles-graphics-framebuffer.cpp.o', '/working/production/gles-graphics-pipeline.cpp.o', '/working/production/gles-graphics-reflection.cpp.o', '/working/production/gles-graphics-render-pass.cpp.o', '/working/production/gles-graphics-render-target.cpp.o', '/working/production/gles-graphics-sampler.cpp.o', '/working/production/gles-graphics-shader.cpp.o', '/working/production/gles-graphics-texture.cpp.o', '/working/production/gles-graphics-pipeline-cache.cpp.o', '/working/production/gles-context.cpp.o', '/working/production/gles-sync-object.cpp.o', '/working/production/gles-framebuffer-state-cache.cpp.o', '/working/production/gles-sync-pool.cpp.o', '/working/production/gles-texture-dependency-checker.cpp.o', '/working/production/web-gl-implementation.cpp.o', '/working/production/web-gles-context.cpp.o', '/working/production/web-gles-rendering.cpp.o', '/working/production/web-gles-sync.cpp.o', '/working/production/web-gles-sync-pool.cpp.o', '/working/production/web-gles-fence.cpp.o', '/working/production/web-gles-memory.cpp.o', '/working/production/web-gles-buffer.cpp.o', '/working/production/web-buffer-memory.cpp.o', '/working/production/web-buffer-storage.cpp.o', '/working/production/web-gles-failure.cpp.o', '/working/production/web-gles-program-cache.cpp.o', '/working/production/environment-variable.cpp.o', '/working/production/web-host.cpp.o', '/working/production/libdali2-core.a', '-sMIN_WEBGL_VERSION=2', '-sMAX_WEBGL_VERSION=2', '-sFULL_ES3=0', '-sASSERTIONS=1', '-sERROR_ON_UNDEFINED_SYMBOLS=1', '-sABORTING_MALLOC=0', '-Wl,--error-limit=0', '-sPTHREAD_POOL_SIZE=1', '-sOFFSCREENCANVAS_SUPPORT=1', '-sALLOW_BLOCKING_ON_MAIN_THREAD=0', '-sMODULARIZE=1', '-sEXPORT_NAME=createDaliWeb', '-sENVIRONMENT=web,worker', '-Wl,-Map=/working/production.map', '-o', '/working/production.js']))
  cases=[c for c in cases if c[0]=="production-link"]
  for name,source,argv in cases:
   js('window.checkResult=null;(async()=>{const t=performance.now();try{if(arguments[1]!==null) await emception.fileSystem.writeFile("/working/"+arguments[0]+".cpp",arguments[1]);const r=arguments[0]==="host-reloc"?await emception._run_process_impl(arguments[2]):await emception.run(...arguments[2]);window.checkResult={...r,ms:performance.now()-t};}catch(e){window.checkResult={error:String(e),ms:performance.now()-t};}})();',[name,source,argv])
   for _ in range(180):
    result=js('return window.checkResult;')
    if result is not None:break
    time.sleep(1)
   item={'name':name,'source':source,'argv':argv,'result':result};results.append(item)
   (logs/'browser-tests.json').write_text(json.dumps(results,indent=2));print(json.dumps(item),flush=True)
   if result is None:break
   if result and result.get('returncode')==0 and name in ['hello','dali-vector']:
    js('window.execution=null;(async()=>{try{const code=await emception.fileSystem.readFile("/working/"+arguments[0]+".js",{encoding:"utf8"});const pre="var Module={print:(s)=>postMessage({stdout:s}),printErr:(s)=>postMessage({stderr:s}),onExit:(c)=>postMessage({exitCode:c}),onAbort:(s)=>postMessage({abort:String(s)})};\\n";const url=URL.createObjectURL(new Blob([pre,code],{type:"text/javascript"}));const messages=[];const w=new Worker(url);w.onmessage=e=>{messages.push(e.data);if(e.data.exitCode!==undefined||e.data.abort){window.execution={messages};w.terminate();URL.revokeObjectURL(url);}};w.onerror=e=>{window.execution={messages,error:e.message};w.terminate();URL.revokeObjectURL(url);};setTimeout(()=>{if(!window.execution){window.execution={messages,timeout:true};w.terminate();URL.revokeObjectURL(url);}},15000);}catch(e){window.execution={error:String(e)};}})();',[name])
    for _ in range(20):
     execution=js('return window.execution;')
     if execution is not None:break
     time.sleep(1)
    item['execution']=execution;(logs/'browser-tests.json').write_text(json.dumps(results,indent=2));print('EXECUTION',name,json.dumps(execution),flush=True)

  resources=js('return performance.getEntriesByType("resource").map(x=>({name:x.name,bytes:x.transferSize,duration:x.duration}));')
  (logs/'resources.json').write_text(json.dumps(resources,indent=2))
 finally:
  if sid:
   try:req('DELETE','/session/'+sid)
   except Exception:pass
  driver.terminate()
  try:driver.wait(timeout=10)
  except subprocess.TimeoutExpired:driver.kill();driver.wait()

server.shutdown()
server.server_close()
