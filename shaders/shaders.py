#!/usr/bin/python3

import os
import subprocess
import sys

if len(sys.argv) < 3 or not os.path.isdir(sys.argv[1]) or not os.path.isdir(sys.argv[2]):
  print('shaders.py <input dir> <output dir>')
  sys.exit()
inDir  = sys.argv[1] if os.path.isabs(sys.argv[1]) else os.path.join(os.getcwd(), sys.argv[1])
outDir = sys.argv[2] if os.path.isabs(sys.argv[2]) else os.path.join(os.getcwd(), sys.argv[2])
for root, dirs, files in os.walk(inDir):
  for file in files:
    if file.endswith('.vert') or file.endswith('.frag'):
      inFile = os.path.join(root, file)
      relPath = os.path.relpath(inFile, inDir)
      outFile = os.path.join(outDir, relPath)
      #outFile, ext = os.path.splitext(outFile)
      outFile = outFile + '.spv'
      if os.path.isfile(outFile) and os.path.getmtime(outFile) >= os.path.getmtime(inFile):
        print('no changes detected in %s, skipping' % file)
        continue
      outPath = os.path.dirname(outFile)
      if not os.path.exists(outPath):
        os.makedirs(outPath)
      print('compiling %s...' % file)
      args = ['glslc', inFile, '-o', outFile]
      result = subprocess.run(args, stdout=subprocess.PIPE)
      if result.stdout:
        print(result.stdout.decode('utf-8'))
print('build completed')