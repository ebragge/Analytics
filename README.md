# Analytics
Multichannel audio recording and analysis sample app that uses Windows Wasapi API. Application supports live analysis, multi channel recording and wav file analysis. Analysis is curretly simply trying to identify high volume peaks in the audio stream.

Record functionality automatically takes all active sound cards from the target device to use and saves stream from them into one file. It is recommendation that all cards are similar such as same USB audio cards. This will ensure that sound sources stay in sync more reliable. 

Analytics is based on this sample: 
https://github.com/Microsoft/Windows-universal-samples/tree/master/Samples/WindowsAudioSession
