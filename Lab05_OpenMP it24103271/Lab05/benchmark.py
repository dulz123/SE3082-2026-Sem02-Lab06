"""Reproducible measurements; all execution is sequential between runs."""
import os, subprocess, re, csv, statistics
from pathlib import Path
p=Path('results');p.mkdir(exist_ok=True)
env=dict(os.environ,OMP_DYNAMIC='FALSE')
rows=[];outside=set();areas=set()
with (p/'benchmarks.txt').open('w') as log:
    for rep in range(1,4):
        jobs=[('serial',1,'')]+[('manual',t,'') for t in (1,2,3,4)]
        # Compare alternative mappings once per repetition at four threads.
        jobs += [('runtime',4,s) for s in ('static','static,1','static,8','dynamic,1','guided,1')]
        for mode,t,sched in jobs:
            e=dict(env,OMP_NUM_THREADS=str(t))
            if sched:e['OMP_SCHEDULE']=sched
            line=subprocess.check_output(['./bin/ex7_mandelbrot',mode],env=e,text=True).strip()
            print(f'repetition={rep} schedule={sched or "default"} {line}',flush=True)
            log.write(f'repetition={rep} schedule={sched or "default"} {line}\n');log.flush()
            d=dict(re.findall(r'(\w+)=([^ ]+)',line))
            if int(d['threads'])!=t:raise RuntimeError('Runtime did not create requested team')
            outside.add(d['outside']);areas.add(d['area'])
            rows.append((mode,t,sched,float(d['time_ms'])))
        for t in (1,2,3,4):
            for exe in ('ex5_fibonacci','ex6_arrays'):
                line=subprocess.check_output(['./bin/'+exe],env=dict(env,OMP_NUM_THREADS=str(t)),text=True)
                if 'PASS' not in line:raise RuntimeError(line)
                log.write(f'repetition={rep} {line}');log.flush()
if len(outside)!=1 or len(areas)!=1:raise RuntimeError('Mandelbrot serial/parallel mismatch')
groups={}
for mode,t,sched,ms in rows:groups.setdefault((mode,t,sched),[]).append(ms)
base=statistics.median(groups[('serial',1,'')])
with (p/'mandelbrot_summary.csv').open('w',newline='') as f:
    w=csv.writer(f);w.writerow(['mode','threads','schedule','median_ms','speedup','efficiency'])
    for (mode,t,sched),times in groups.items():
        median=statistics.median(times)
        w.writerow([mode,t,sched,median,base/median,base/median/t])
(p/'validation.txt').write_text(f'PASS: identical outside count {next(iter(outside))} and area {next(iter(areas))} across all Mandelbrot runs.\nPASS: all Fibonacci and array comparisons.\n')
print((p/'validation.txt').read_text())
