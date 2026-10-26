import json,os,socket,subprocess,time,urllib.request
from pathlib import Path
logs=Path('${DALI_WEB_ROOT}/logs/emception/assessment-002')
logs.mkdir(parents=True,exist_ok=False)
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
  cases=[('hello','#include <iostream>\n#include <vector>\nint main(){std::vector<int> v{1,2,3};std::cout << "RESULT:" << v.size() << std::endl;}\n',['em++','hello.cpp','-std=c++17','-O0','-sSINGLE_FILE=1','-sEXIT_RUNTIME=1','-o','hello.js'])]
  for name,source,argv in cases:
   js('window.checkResult=null;(async()=>{const t=performance.now();try{if(arguments[1]!==null) await emception.fileSystem.writeFile("/working/"+arguments[0]+".cpp",arguments[1]);const r=arguments[0]==="host-reloc"?await emception._run_process_impl(arguments[2]):await emception.run(...arguments[2]);window.checkResult={...r,ms:performance.now()-t};}catch(e){window.checkResult={error:String(e),ms:performance.now()-t};}})();',[name,source,argv])
   for _ in range(180):
    result=js('return window.checkResult;')
    if result is not None:break
    time.sleep(1)
   item={'name':name,'source':source,'argv':argv,'result':result};results.append(item)
   (logs/'browser-tests.json').write_text(json.dumps(results,indent=2));print(json.dumps(item),flush=True)
   if result is None:break
   if result and result.get('returncode')==0 and name == 'hello':
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
