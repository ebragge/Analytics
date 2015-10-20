# Analytics
Multichannel audio recording and analysis sample that uses Windows Wasapi API. Application supports live analysis, multi channel recording and wav file analysis. At the moment analytics identifies high volume peaks in the audio stream. The goal is to add calculations that would estimate phase shifts between channels.

Notice that recording functionality automatically takes all active sound cards from the target device to use and saves stream from them into a file. It is recommendation that all cards are similar such as same USB audio cards. This will ensure that sound sources stay in sync. 

Analytics is based on this sample: 
https://github.com/Microsoft/Windows-universal-samples/tree/master/Samples/WindowsAudioSession
