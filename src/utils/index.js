export function csv2Array(data) {    
    var array = [];
    var lines = data.split('\n');
    for(var l = 0; l < lines.length; l++)
        array.push(lines[l].split(',').map(el => Number.isFinite(parseFloat(el)) ? parseFloat(el) : el));
    return array;
};

export function readFile(file, type){
    return new Promise((resolve, reject) => {
        if(file){            
            var reader = new FileReader();
            var loaded = false;
            reader.onload = content => {                
                var result = {};
                if(type === "json")
                    result = JSON.parse(content.target.result);        
                else
                    result = csv2Array(content.target.result);                
                loaded = true;
                resolve(result);
            };
            setTimeout(function(){
                if(!loaded) reject("Timeout");
            }, 1000);
            reader.readAsText(file);
        }
    });
};