import sys
import ROOT as r

r.gROOT.SetBatch()
r.gStyle.SetOptStat(0)

labels = {
        'legacy.root': 'RAW',
        'reemul.root': 'reemulated',
        'reemul_uncompressed.root': 'uncompressed'
}

def plot(fn):
    f = r.TFile(fn)
    c = r.TCanvas()
    t = f.Get("study/met")

    t.Draw("tp:rh>>hist(50,0,600,50,0,600)", "", "COLZ")
    r.gDirectory.Get("hist").SetTitle(";RH MET;TP MET")
    c.SetLogz()
    c.SaveAs(fn.replace(".root", "_met.pdf"))

    t.Draw("tp:rh>>hist(50,0,200,50,0,200)", "", "COLZ")
    r.gDirectory.Get("hist").SetTitle(";RH MET;TP MET")
    c.SetLogz()
    c.SaveAs(fn.replace(".root", "_met_low.pdf"))

    t = f.Get("comp/matches")
    t.Draw("TP_energy:RH_energy>>hist(50,0,200,50,0,200)", "", "COLZ")
    r.gDirectory.Get("hist").SetTitle(";RH #sum E_{T};TP #sum E_{T}")
    c.SetLogz()
    c.SaveAs(fn.replace(".root", "_et.pdf"))

    t.Draw("TP_energy:RH_energy>>hist(50,0,10,50,0,10)", "", "COLZ")
    r.gDirectory.Get("hist").SetTitle(";RH #sum E_{T};TP #sum E_{T}")
    c.SetLogz()
    c.SaveAs(fn.replace(".root", "_et_low.pdf"))

    f.Close()

def plot_composite(fns):
    c = r.TCanvas()

    hists = []
    for fn in fns:
        f = r.TFile(fn)
        t = f.Get("comp/tps")
        t.Draw("et>>hist(100,0,200)", "", "")
        h = r.gDirectory.Get("hist")
        h.SetTitle(";TP E_{T};Count");
        h.SetDirectory(0)
        h.SetName(fn)
        h.Scale(100. / t.GetEntries())
        hists.append(h)
        f.Close()

    opt = ""
    l = r.TLegend(.2, .2, .7, .9)
    hists = sorted(hists, key=lambda h: h.GetMaximum(), reverse=True)
    for color, h in zip([r.kRed, r.kBlue, r.kBlack], hists):
        h.SetName(labels[h.GetName()])
        h.SetLineColor(color)
        h.Draw(opt)
        l.AddEntry(h.GetName(), h.GetName(), "l")
        opt = "same"
    c.SetLogy()
    l.Draw()
    c.SaveAs("tp_et.pdf")

    hists = []
    for fn in fns:
        f = r.TFile(fn)
        t = f.Get("study/met")
        t.Draw("(rh-tp)/rh>>hist(101,-10,2)", "", "")
        h = r.gDirectory.Get("hist")
        h.SetTitle(";(RH-TP)/RH MET;Count");
        h.SetDirectory(0)
        h.SetName(fn)
        h.Scale(100. / t.GetEntries())
        hists.append(h)
        f.Close()

    opt = ""
    l = r.TLegend(.1, .7, .5, .9)
    hists = sorted(hists, key=lambda h: h.GetMaximum(), reverse=True)
    for color, h in zip([r.kRed, r.kBlue, r.kBlack], hists):
        h.SetName(labels[h.GetName()])
        h.SetLineColor(color)
        h.Draw(opt)
        l.AddEntry(h.GetName(), h.GetName(), "l")
        opt = "same"
    c.SetLogy()
    l.Draw()
    c.SaveAs("tp_rh_rel_diff_log.pdf")

    hists = []
    for fn in fns:
        f = r.TFile(fn)
        t = f.Get("study/met")
        t.Draw("(rh-tp)/rh>>hist(30,-1.1,1.1)", "", "")
        h = r.gDirectory.Get("hist")
        h.SetTitle(";(RH-TP)/RH MET;Count");
        h.SetDirectory(0)
        h.SetName(fn)
        h.Scale(100. / t.GetEntries())
        hists.append(h)
        f.Close()

    opt = ""
    l = r.TLegend(.1, .7, .5, .9)
    hists = sorted(hists, key=lambda h: h.GetMaximum(), reverse=True)
    for color, h in zip([r.kRed, r.kBlue, r.kBlack], hists):
        h.SetName(labels[h.GetName()])
        h.SetLineColor(color)
        h.Draw(opt)
        l.AddEntry(h.GetName(), h.GetName(), "l")
        opt = "same"
    c.SetLogy(False)
    l.Draw()
    c.SaveAs("tp_rh_rel_diff.pdf")

plot_composite(sys.argv[1:])

for fn in sys.argv[1:]:
    plot(fn)
