const fs = require('fs');
const { PerformanceObserver, performance } = require('perf_hooks');

const data = fs.readFileSync('./test-big-1.json', 'utf8');
const start = performance.now();

const pretty = JSON.stringify(JSON.parse(data), null, 2);

fs.writeFileSync('./test-big-1-pretty.json', pretty);

const end = performance.now();
console.log(`Took ${end - start}ms`);