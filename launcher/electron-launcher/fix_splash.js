const fs=require('fs');const path=require('path');const{execSync}=require('child_process');
const LAUNCHER_DIR=__dirname;
const rhExe=path.join(LAUNCHER_DIR,'ResourceHacker.exe');
const ugBmp=path.join(LAUNCHER_DIR,'ug_splash.bmp');
const settings=JSON.parse(fs.readFileSync(path.join(LAUNCHER_DIR,'settings.json'),'utf8'));
const gtaPath=settings.gta_path;
if(!gtaPath){console.log('GTA path not found!');process.exit(1);}
console.log('GTA Path:',gtaPath);
console.log('Resource Hacker:',rhExe,'exists=',fs.existsSync(rhExe));
console.log('UG Splash BMP:',ugBmp,'exists=',fs.existsSync(ugBmp));
if(!fs.existsSync(rhExe)){console.log('ERROR: ResourceHacker.exe not found!');process.exit(1);}
if(!fs.existsSync(ugBmp)){console.log('ERROR: ug_splash.bmp not found!');process.exit(1);}
const filesToPatch=['samp.exe','samp.dll'];
for(const fname of filesToPatch){
const fpath=path.join(gtaPath,fname);
if(!fs.existsSync(fpath)){console.log(fname+' not found, skipping');continue;}
console.log('\n=== Patching '+fname+' ===');
const backup=fpath+'.ug_backup';
if(!fs.existsSync(backup)){fs.copyFileSync(fpath,backup);console.log('Backed up to '+backup);}
const resFile=path.join(LAUNCHER_DIR,fname+'_resources.txt');
try{execSync(''+rhExe+' -open '+fpath+' -save '+resFile+' -action list -mask ,,',{timeout:15000,windowsHide:true});}catch(e){}
let bitmapIds=[];
if(fs.existsSync(resFile)){
try{const txt=fs.readFileSync(resFile,'utf8');
const lines=txt.split('\n');
for(const line of lines){
const m=line.match(/BITMAP\\s+(\\d+)/i);
if(m)bitmapIds.push(parseInt(m[1]));
}
if(bitmapIds.length===0){
for(const line of lines){
const m=line.match(/BITMAP/i);
if(m){
const parts=line.trim().split(/\\s+/);
for(let i=0;i<parts.length;i++){if(parts[i].match(/^\\d+$/)){bitmapIds.push(parseInt(parts[i]));}}
}
}
}
console.log('Found bitmap IDs:',bitmapIds.length>0?bitmapIds.join(', '):'none from parsing, trying defaults');
console.log('Resource list first 50 lines:');
console.log(lines.slice(0,50).join('\n'));
try{fs.unlinkSync(resFile);}catch(e){}
}catch(e){console.log('Error reading resource list:',e.message);}
}
if(bitmapIds.length===0)bitmapIds=[100,101,102,103,104,105,106,107,108,109,110];
let patched=false;
for(const id of bitmapIds){
const patchedFile=path.join(LAUNCHER_DIR,'ug_patched_'+fname);
try{try{fs.unlinkSync(patchedFile);}catch(e){}
execSync(''+rhExe+' -open '+fpath+' -save '+patchedFile+' -action addoverwrite -res '+ugBmp+' -mask BITMAP,'+id+',0',{timeout:30000,windowsHide:true});
if(fs.existsSync(patchedFile)){
const stat=fs.statSync(patchedFile);
if(stat.size>10000){
fs.copyFileSync(patchedFile,fpath);
console.log('PATCHED '+fname+' resource BITMAP,'+id+' ('+stat.size+' bytes)');
patched=true;
}else{console.log('Patched file too small for ID '+id);}
try{fs.unlinkSync(patchedFile);}catch(e){}
}
}catch(e){console.log('Failed ID '+id+': '+e.message);}
}
if(!patched){console.log('WARNING: Could not patch any bitmap in '+fname);}
}
console.log('\n=== DONE ===');
console.log('Now run the launcher and click PRIKLJUCI SE!');
