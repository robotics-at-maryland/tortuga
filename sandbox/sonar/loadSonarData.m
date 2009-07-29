function dat = loadSonarData(fname)
fid = fopen(fname);
if fid == -1
  error('failed to open file %s', fname);
end
dat = fread(fid, [4, Inf], 'int16');
fclose(fid);
end
