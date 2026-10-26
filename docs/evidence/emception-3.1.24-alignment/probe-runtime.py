import json,os,socket,subprocess,time,urllib.request
from pathlib import Path
import sys
mode=sys.argv[1]
logs=Path('${DALI_WEB_ROOT}/logs/emception/alignment-3124-runtime-'+mode)
logs.mkdir(parents=True,exist_ok=False)
profiles=logs/'profiles';profiles.mkdir(exist_ok=True)
with socket.socket() as s:s.bind(('127.0.0.1',0));port=s.getsockname()[1]
endpoint=f'http://127.0.0.1:{port}'
def req(method,path,data=None):
 q=urllib.request.Request(endpoint+path,data=None if data is None else json.dumps(data).encode(),method=method,headers={'Content-Type':'application/json'})
 with urllib.request.urlopen(q,timeout=40) as r:return json.load(r)['value']
import http.server,threading,functools
assets=Path('${DALI_WEB_ROOT}/logs/emception/alignment-3124-assets')
class Handler(http.server.SimpleHTTPRequestHandler):
 def end_headers(self):
  if mode=='headers':
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
  req('POST',base+'/url',{'url':f'http://127.0.0.1:{server.server_port}/runtime-check.html?sw={1 if mode=="sw" else 0}'})
  def js(script,args=[]):return req('POST',base+'/execute/sync',{'script':script,'args':args})
  for _ in range(90):
   result=js('return window.runtimeResult || null;')
   if result and result.get('done'):break
   time.sleep(1)
  (logs/'runtime-result.json').write_text(json.dumps(result,indent=2));print(json.dumps(result),flush=True)
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
