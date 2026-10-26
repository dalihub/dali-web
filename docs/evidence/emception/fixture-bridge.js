const worker = new Worker('emception.worker.bundle.worker.js');
let next = 0; const pending = new Map();
worker.onmessage = ({data}) => { const p = pending.get(data.id); if (!p) return; pending.delete(data.id); if (data.type === 'RAW') p.resolve(data.value); else p.reject(new Error(JSON.stringify(data.value))); };
function proxy(path) { return new Proxy(function(){}, { get(_,key) { if (key === 'then') return undefined; return proxy([...path,key]); }, apply(_,__,args) { return new Promise((resolve,reject) => { const id=String(++next);pending.set(id,{resolve,reject});worker.postMessage({id,type:'APPLY',path,argumentList:args.map(value=>({type:'RAW',value}))}); }); } }); }
window.emception = proxy([]);
emception.init().then(()=>{document.querySelector('#compile').disabled=false;document.querySelector('#compile').textContent='Ready';}).catch(e=>document.body.textContent=String(e));
