window.probe={phase:'boot',cases:[],done:false,logs:[]};
for(const name of ['log','warn','error']){const orig=console[name];console[name]=(...args)=>{window.probe.logs.push({level:name,text:args.map(String).join(' ')});if(window.probe.logs.length>120)window.probe.logs.shift();orig(...args);};}
try {
 const {createEmception}=await import('/packages/@gameguild/emception-browser/dist/createEmception.js');
 const em=await createEmception({tty:'none',manifestUrl:'/cdn/manifest.json'});
 for(const [name,args] of [['clang-version',['clang++','--version']],['tiny-object',['clang++','-c','/home/user/tiny.cpp','-o','/home/user/tiny.o']]]){
  window.probe.phase=name;
  if(name==='tiny-object')await em.workspace.writeFile('/home/user/tiny.cpp','extern "C" int add(int a,int b){return a+b;}');
  const result=await Promise.race([em.run('clang++',args),new Promise(resolve=>setTimeout(()=>resolve({timedOut:true}),45000))]);
  let object=null;if(name==='tiny-object'&&!result.timedOut){try{const b=await em.workspace.readFile('/home/user/tiny.o');object={bytes:b.byteLength,head:Array.from(b.slice(0,8))};}catch(e){object={error:String(e)};}}
  window.probe.cases.push({name,args,result,object});if(result.timedOut)break;
 }
 em.dispose();window.probe.done=true;
}catch(e){window.probe.error=String(e);window.probe.done=true;}
