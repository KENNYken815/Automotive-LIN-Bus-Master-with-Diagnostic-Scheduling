"""Summarize LIN simulator CSV traces."""
import argparse,csv,collections
p=argparse.ArgumentParser();p.add_argument('trace');a=p.parse_args()
with open(a.trace,newline='',encoding='utf-8') as f: rows=list(csv.DictReader(f))
c=collections.Counter(r['result'] for r in rows);n=len(rows);print(f'frames={n} results={dict(c)} timeouts={c["timeout"]} bus_slot_utilization={100*(n-c["timeout"])/n:.1f}%')
