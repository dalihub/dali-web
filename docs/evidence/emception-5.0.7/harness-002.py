import json,re,time,socket,subprocess,urllib.request,http.server,threading
from pathlib import Path
root=Path('${DALI_WEB_ROOT}/logs/emception/gameguild-001')
run=root/'browser-002';run.mkdir(exist_ok=False)
class Handler(http.server.BaseHTTPRequestHandler):
 def do_GET(self):
  path=self.path.split('?')[0]
  if path=='/':data=b'<script type="module" src="/probe.js"></script>';mime='text/html'
  elif path=='/probe.js':data=(root/'probe.js').read_bytes();mime='text/javascript'
  else:
   target=root/'packages/emception/cdn'/path[5:] if path.startswith('/cdn/') else root/path.lstrip('/')
   if not target.is_file() and target.with_suffix('.js').is_file():target=target.with_suffix('.js')
   if not target.is_file():self.send_error(404);return
   data=target.read_bytes();mime='application/octet-stream'
   if target.suffix=='.json':mime='application/json'
   if target.suffix=='.wasm':mime='application/wasm'
   if target.suffix in ['.js','.mjs']:
    mime='text/javascript';s=data.decode()
    for name in ['emception','@gameguild/emception-xterm']:
     s=s.replace("from '"+name+"'", "from '/packages/"+name+"/dist/index.js'")
    data=s.encode()
   if self.path.endswith('.py?raw'):data=('export default '+json.dumps(data.decode())+';').encode();mime='text/javascript'
  self.send_response(200);self.send_header('Content-Type',mime);self.send_header('Content-Length',str(len(data)))
  self.send_header('Cross-Origin-Opener-Policy','same-origin');self.send_header('Cross-Origin-Embedder-Policy','require-corp');self.end_headers();self.wfile.write(data)
 def log_message(self,fmt,*a):
  with (run/'http.log').open('a') as f:f.write(fmt%a+'\n')
server=http.server.ThreadingHTTPServer(('127.0.0.1',0),Handler);threading.Thread(target=server.serve_forever,daemon=True).start()
with socket.socket() as s:s.bind(('127.0.0.1',0));port=s.getsockname()[1]
endpoint=f'http://127.0.0.1:{port}'
def req(method,path,data=None):
 q=urllib.request.Request(endpoint+path,data=None if data is None else json.dumps(data).encode(),method=method,headers={'Content-Type':'application/json'})
 with urllib.request.urlopen(q,timeout=40) as r:return json.load(r)['value']
sid=None;profiles=run/'profiles';profiles.mkdir()
with (run/'geckodriver.log').open('w') as log:
 driver=subprocess.Popen(['geckodriver','--port',str(port),'--profile-root',str(profiles)],stdout=log,stderr=log)
 try:
  for _ in range(100):
   try:req('GET','/status');break
   except OSError:time.sleep(.1)
  session=req('POST','/session',{'capabilities':{'alwaysMatch':{'browserName':'firefox','moz:firefoxOptions':{'args':['-headless'],'prefs':{'network.proxy.type':1,'network.proxy.http':'127.0.0.1','network.proxy.http_port':9,'network.proxy.ssl':'127.0.0.1','network.proxy.ssl_port':9,'network.proxy.no_proxies_on':'localhost,127.0.0.1','network.proxy.failover_direct':False}}}}})
  sid=session['sessionId'];base='/session/'+sid;(run/'capabilities.json').write_text(json.dumps(session,indent=2))
  req('POST',base+'/url',{'url':f'http://127.0.0.1:{server.server_port}/'})
  for i in range(240):
   result=req('POST',base+'/execute/sync',{'script':'return window.probe || {loading:true};','args':[]})
   (run/'result.json').write_text(json.dumps(result,indent=2))
   if i%15==0:print(i,json.dumps(result)[:600],flush=True)
   if result.get('done'):break
   time.sleep(1)
  print(json.dumps(result),flush=True)
 finally:
  if sid:
   try:req('DELETE','/session/'+sid)
   except Exception:pass
  driver.terminate()
  try:driver.wait(timeout=10)
  except subprocess.TimeoutExpired:driver.kill();driver.wait()
  server.shutdown();server.server_close()
