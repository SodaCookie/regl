var fs = require('fs');
fs.writeFileSync('reglnative.pc', 'prefix=' + __dirname + '\n' + fs.readFileSync('reglnative.pc', 'utf8'));
