rm Debug/HcalMetStudy/python/files_*.py
wget -O Debug/HcalMetStudy/python/files_raw.py "https://cmsweb.cern.ch/das/makepy?dataset=/MET/Run2015C-v1/RAW&instance=prod/global"
wget -O Debug/HcalMetStudy/python/files_reco.py "https://cmsweb.cern.ch/das/makepy?dataset=/MET/Run2015C-PromptReco-v1/RECO&instance=prod/global"
