"""Generate analytical reference CSVs; these are not outputs from the C++ binary."""
import csv
import math
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "results"
DATA = ROOT / "data"
OUT.mkdir(exist_ok=True)

def exact(x):
    return 100 - 80*x + 500*x*(1-x)

with (OUT / "fea_N40_analytical_reference.csv").open("w", newline="", encoding="utf-8") as f:
    w = csv.writer(f); w.writerow(["x_m", "temperature_C"])
    for i in range(41):
        x = i/40; w.writerow([f"{x:.8f}", f"{exact(x):.8f}"])

with (DATA / "sample_temperature_synthetic.csv").open("w", newline="", encoding="utf-8") as f:
    w = csv.writer(f); w.writerow(["node_id", "x_m", "y_m", "z_m", "temperature_C"])
    for i in range(21):
        x = i/20
        w.writerow([i+1, f"{x:.4f}", "0.0000", "0.0000", f"{exact(x):.4f}"])

params = [0.5, 0.0, 1.0, 0.0]
def loss(p):
    w,b,v,c = p; h=math.tanh(w+b); e=v*h+c-2; return .5*e*e
def grad(p):
    w,b,v,c=p; h=math.tanh(w+b); e=v*h+c-2; z=e*v*(1-h*h)
    return [z,z,e*h,e]
with (OUT / "nn_loss_reference.csv").open("w", newline="", encoding="utf-8") as f:
    wr=csv.writer(f); wr.writerow(["step", "loss"]); wr.writerow([0, f"{loss(params):.12g}"])
    for step in range(1,101):
        g=grad(params); params=[p-.1*d for p,d in zip(params,g)]
        wr.writerow([step, f"{loss(params):.12g}"])

def svg_plot(path, title, values, x_label, y_label, log_y=False):
    width,height=800,420; left,right,top,bottom=80,30,50,65
    ys=[math.log10(max(v,1e-16)) if log_y else v for v in values]
    ymin,ymax=min(ys),max(ys); ymin=ymin-(ymax-ymin)*.08; ymax=ymax+(ymax-ymin)*.08 or 1
    pts=[]
    for i,y in enumerate(ys):
        x=left+i*(width-left-right)/max(1,len(ys)-1); py=top+(ymax-y)*(height-top-bottom)/(ymax-ymin)
        pts.append(f"{x:.1f},{py:.1f}")
    svg=[f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}">','<rect width="100%" height="100%" fill="white"/>',f'<text x="{width/2}" y="28" text-anchor="middle" font-family="Arial" font-size="20">{title}</text>',f'<path d="M{left} {top} V{height-bottom} H{width-right}" fill="none" stroke="#333"/>',f'<polyline points="{" ".join(pts)}" fill="none" stroke="#1769aa" stroke-width="3"/>',f'<text x="{width/2}" y="{height-15}" text-anchor="middle" font-family="Arial" font-size="14">{x_label}</text>',f'<text x="20" y="{height/2}" transform="rotate(-90 20 {height/2})" text-anchor="middle" font-family="Arial" font-size="14">{y_label}</text>','</svg>']
    (OUT/path).write_text("\n".join(svg), encoding="utf-8")

svg_plot("nn_loss_reference.svg", "Analytical reference: single-sample NN loss", [loss([.5,0,1,0])] + [float(row[1]) for row in list(csv.reader((OUT/"nn_loss_reference.csv").open()))[1:]], "Gradient descent step", "Loss")
svg_plot("fea_convergence_reference.svg", "Analytical reference: midpoint error convergence", [1.25,.3125,.078125,.01953125], "Element count (10, 20, 40, 80)", "Max absolute error (C)", True)
print("Generated analytical references; they are not C++ runtime evidence.")
