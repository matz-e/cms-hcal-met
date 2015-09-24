import sys
import ROOT as r

def plot(fn):
    f = r.TFile(fn)
    c = r.TCanvas()
    t = f.Get("study/met")
    t.Draw("tp:rh>>hist(50,0,200,50,0,200)", "", "COLZ")
    c.SetLogz()
    c.SaveAs(fn.replace(".root", "_met.png"))

    t = f.Get("comp/matches")
    t.Draw("TP_energy:RH_energy>>hist(50,0,200,50,0,200)", "", "COLZ")
    c.SetLogz()
    c.SaveAs(fn.replace(".root", "_et.png"))

for fn in sys.argv[1:]:
    plot(fn)
