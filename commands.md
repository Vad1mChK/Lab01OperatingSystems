```shell
runnb ema-sort-int full-benchmark /home/vad1mchk/Documents/testfile/inp /home/vad1mchk/Documents/testfile/outp 1

runnb ram-sort-int full-benchmark /home/vad1mchk/Documents/testfile/inp /home/vad1mchk/Documents/testfile/outp 1

run ema-ram-sort-int generate /home/vad1mchk/Documents/testfile/inp 128
runnb ema-ram-sort-int sort /home/vad1mchk/Documents/testfile/inp /home/vad1mchk/Documents/testfile/outp 32 2 4 
run ema-ram-sort-int check /home/vad1mchk/Documents/testfile/outp.ram.0
run ema-ram-sort-int check /home/vad1mchk/Documents/testfile/outp.ema.0
```