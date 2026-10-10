"""Deterministic LIN schedule/slave simulator and CSV logger."""
import argparse,csv,random

def run(cycles,missing_slave):
    rng=random.Random(11);rows=[];now=0;stats={'sent':0,'received':0,'timeouts':0,'recovered':0}
    slots=[(0x10,0,10),(0x11,1,10),(0x12,2,10),(0x3C,3,20)]
    for cycle in range(cycles):
        for fid,node,delay in slots:
            now += delay*1000;stats['sent']+=1
            diagnostic=node==3;missing=(node==missing_slave)
            present=diagnostic or not missing
            response_delay=rng.randint(250,800)
            result='diagnostic' if diagnostic else ('ok' if present and response_delay<2000 else 'timeout')
            if not diagnostic and result=='ok':stats['received']+=1
            if result=='timeout':stats['timeouts']+=1;stats['recovered']+=1
            rows.append({'time_us':now,'frame_id':f'0x{fid:02X}','slot':node,'result':result,'response_delay_us':response_delay if present else '', 'cycle':cycle})
    return rows,stats

def main():
    p=argparse.ArgumentParser();p.add_argument('--cycles',type=int,default=20);p.add_argument('--missing-slave',type=int,choices=[0,1,2],default=2);p.add_argument('--csv',default='lin_trace.csv');a=p.parse_args()
    rows,stats=run(a.cycles,a.missing_slave)
    with open(a.csv,'w',newline='',encoding='utf-8') as f:w=csv.DictWriter(f,fieldnames=rows[0].keys());w.writeheader();w.writerows(rows)
    print('LIN simulation summary:',stats,'trace=',a.csv)
if __name__=='__main__':main()
