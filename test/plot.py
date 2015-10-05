import sys
import ROOT as r

r.gROOT.SetBatch()
r.gStyle.SetOptStat(0)

labels = {
        'legacy.root': 'RAW',
        'reemul.root': 'reemulated',
        'reemul_uncompressed.root': 'uncompressed'
}

def plot_met(fn, which=""):
    f = r.TFile(fn)
    c = r.TCanvas()
    t = f.Get("study/met")

    t.Draw("tp{0}:rh{0}>>hist(50,0,600,50,0,600)".format(which), "", "COLZ")
    r.gDirectory.Get("hist").SetTitle("{0};RH MET;TP MET".format(labels[fn]))
    c.SetLogz()
    c.SaveAs(fn.replace(".root", "_met{0}.pdf".format(which)))

    t.Draw("tp{0}:rh{0}>>hist(50,0,200,50,0,200)".format(which), "", "COLZ")
    r.gDirectory.Get("hist").SetTitle("{0};RH MET;TP MET".format(labels[fn]))
    c.SetLogz()
    c.SaveAs(fn.replace(".root", "_met{0}_low.pdf".format(which)))

def plot_matches(fn):
    f = r.TFile(fn)
    c = r.TCanvas()
    t = f.Get("comp/matches")

    t.Draw("TP_energy:RH_energy>>hist(50,0,200,50,0,200)", "", "COLZ")
    r.gDirectory.Get("hist").SetTitle(";RH E_{T};TP E_{T}")
    c.SetLogz()
    c.SaveAs(fn.replace(".root", "_et.pdf"))

    t.Draw("TP_energy:RH_energy>>hist(50,0,10,50,0,10)", "", "COLZ")
    r.gDirectory.Get("hist").SetTitle(";RH E_{T};TP E_{T}")
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
        # h.Scale(100. / h.Integral())
        hists.append(h)
        f.Close()

    opt = ""
    l = r.TLegend(.5, .7, .9, .9)
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
        t.Draw("tp>>hist(100,0,500)", "", "")
        h = r.gDirectory.Get("hist")
        h.SetTitle(";TP E_{T};Count");
        h.SetDirectory(0)
        h.SetName(fn)
        # h.Scale(100. / h.Integral())
        hists.append(h)
        f.Close()

    opt = ""
    l = r.TLegend(.5, .7, .9, .9)
    hists = sorted(hists, key=lambda h: h.GetMaximum(), reverse=True)
    for color, h in zip([r.kRed, r.kBlue, r.kBlack], hists):
        h.SetName(labels[h.GetName()])
        h.SetLineColor(color)
        h.Draw(opt)
        l.AddEntry(h.GetName(), h.GetName(), "l")
        opt = "same"
    c.SetLogy()
    l.Draw()
    c.SaveAs("tp_met.pdf")

def plot_pull(fns, selection, stub, which=""):
    c = r.TCanvas()

    hists = []
    for fn in fns:
        f = r.TFile(fn)
        t = f.Get("study/met")
        t.Draw("(tp{0}-rh{0})/rh{0}>>hist(101,-10,2)".format(which), selection, "")
        h = r.gDirectory.Get("hist")
        h.SetTitle(";(TP-RH)/RH MET;Count");
        h.SetDirectory(0)
        h.SetName(fn)
        h.Scale(100. / t.GetEntries())
        hists.append(h)
        f.Close()

    opt = ""
    l = r.TLegend(.1, .7, .4, .9)
    hists = sorted(hists, key=lambda h: h.GetMaximum(), reverse=True)
    for color, h in zip([r.kRed, r.kBlue, r.kBlack], hists):
        h.SetName(labels[h.GetName()])
        h.SetLineColor(color)
        h.Draw(opt)
        l.AddEntry(h.GetName(), h.GetName(), "l")
        opt = "same"
    c.SetLogy()
    l.Draw()
    c.SaveAs("tp_rh_rel_diff{1}_{0}_log.pdf".format(stub, which))

    hists = []
    for fn in fns:
        f = r.TFile(fn)
        t = f.Get("study/met")
        t.Draw("(rh{0}-tp{0})/rh{0}>>hist(30,-1.1,1.1)".format(which), selection, "")
        h = r.gDirectory.Get("hist")
        h.SetTitle(";(RH-TP)/RH MET;Count");
        h.SetDirectory(0)
        h.SetName(fn)
        h.Scale(100. / t.GetEntries())
        hists.append(h)
        f.Close()

    opt = ""
    l = r.TLegend(.1, .7, .4, .9)
    hists = sorted(hists, key=lambda h: h.GetMaximum(), reverse=True)
    for color, h in zip([r.kRed, r.kBlue, r.kBlack], hists):
        h.SetName(labels[h.GetName()])
        h.SetLineColor(color)
        h.Draw(opt)
        l.AddEntry(h.GetName(), h.GetName(), "l")
        opt = "same"
    c.SetLogy(False)
    l.Draw()
    c.SaveAs("tp_rh_rel_diff{1}_{0}.pdf".format(stub, which))

plot_composite(sys.argv[1:])

for i in range(4):
    w = "_eta_cut_{0}".format(i) if i < 3 else ""
    plot_pull(sys.argv[1:], "", "no_cut", which=w)
    plot_pull(sys.argv[1:], "tp > 50", "gt50", which=w)
    plot_pull(sys.argv[1:], "tp > 100", "gt100", which=w)
    plot_pull(sys.argv[1:], "tp > 150", "gt150", which=w)

for fn in sys.argv[1:]:
    plot_met(fn)
    plot_met(fn, "_eta_cut_0")
    plot_met(fn, "_eta_cut_1")
    plot_met(fn, "_eta_cut_2")
    plot_matches(fn)
