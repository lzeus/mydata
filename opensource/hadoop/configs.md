```
mapreduce.task.io.sort.mb //each map task has a circular memory buffer that it writes the output to
mapreduce.map.sort.spill.percent //the contents of the buffer reach a certain threshold size, a backgroud thread will start to spill the contents to disk
```
