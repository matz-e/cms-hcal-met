import sys
import ROOT as r

r.gROOT.SetBatch()

labels = {
        'legacy.root': 'RAW',
        'reemul.root': 'reemulated',
        'reemul_uncompressed.root': 'uncompressed'
}

def plot(fn):
    f = r.TFile(fn)
    c = r.TCanvas()
    t = f.Get("study/met")
    t.Draw("tp:rh>>hist(50,0,200,50,0,200)", "", "COLZ")
    c.SetLogz()
    c.SaveAs(fn.replace(".root", "_met.pdf"))

    t = f.Get("comp/matches")
    t.Draw("TP_energy:RH_energy>>hist(50,0,200,50,0,200)", "", "COLZ")
    c.SetLogz()
    c.SaveAs(fn.replace(".root", "_et.pdf"))

    t.Draw("TP_energy:RH_energy>>hist(50,0,10,50,0,10)", "", "COLZ")
    c.SetLogz()
    c.SaveAs(fn.replace(".root", "_et_low.pdf"))

    f.Close()

def plot_composite(fns):
    hists = []
    for fn in fns:
        f = r.TFile(fn)
        t = f.Get("comp/tps")
        t.Draw("et>>hist(100,0,200)", "", "")
        h = r.gDirectory.Get("hist")
        h.SetDirectory(0)
        h.SetName(fn)
        hists.append(h)
        f.Close()

    opt = ""
    c = r.TCanvas()
    l = r.TLegend(.2, .9, .2, .9)
    for color, h in zip([r.kRed, r.kBlue, r.kBlack], hists):
        h.SetName(labels[h.GetName()])
        h.SetLineColor(color)
        h.Draw(opt)
        l.AddEntry(h.GetName(), h.GetName(), "l")
        opt = "same"
    c.SetLogy()
    print 'legen - wait for it - dary'
    l.Draw()
    c.SaveAs("tp_et.pdf")

plot_composite(sys.argv[1:])

for fn in sys.argv[1:]:
    plot(fn)
