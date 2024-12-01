```shell
runnb ema-sort-int full-benchmark /home/vad1mchk/Documents/testfile/inp /home/vad1mchk/Documents/testfile/outp 1

runnbfor i=0:3 ram-sort-int full-benchmark /home/vad1mchk/Documents/testfile/inp.<i> /home/vad1mchk/Documents/testfile/outp.<i> 1
runnbfor i=0:7 ram-sort-int full-benchmark /home/vad1mchk/Documents/testfile/inp.<i> /home/vad1mchk/Documents/testfile/outp.<i> 1
runnbfor i=0:11 ram-sort-int full-benchmark /home/vad1mchk/Documents/testfile/inp.<i> /home/vad1mchk/Documents/testfile/outp.<i> 1
runnbfor i=0:15 ram-sort-int full-benchmark /home/vad1mchk/Documents/testfile/inp.<i> /home/vad1mchk/Documents/testfile/outp.<i> 1

runnbfor i=0:3 ema-sort-int full-benchmark /home/vad1mchk/Documents/testfile/inp.<i> /home/vad1mchk/Documents/testfile/outp.<i> 1
runnbfor i=0:7 ema-sort-int full-benchmark /home/vad1mchk/Documents/testfile/inp.<i> /home/vad1mchk/Documents/testfile/outp.<i> 1
runnbfor i=0:11 ema-sort-int full-benchmark /home/vad1mchk/Documents/testfile/inp.<i> /home/vad1mchk/Documents/testfile/outp.<i> 1
runnbfor i=0:15 ema-sort-int full-benchmark /home/vad1mchk/Documents/testfile/inp.<i> /home/vad1mchk/Documents/testfile/outp.<i> 1

run ema-ram-sort-int generate /home/vad1mchk/Documents/testfile/inp 128
runnb ema-ram-sort-int sort /home/vad1mchk/Documents/testfile/inp /home/vad1mchk/Documents/testfile/outp 32 2 4 
run ema-ram-sort-int check /home/vad1mchk/Documents/testfile/outp.ram.0
run ema-ram-sort-int check /home/vad1mchk/Documents/testfile/outp.ema.0
```