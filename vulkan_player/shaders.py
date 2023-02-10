import os
import sys

sys.path.insert(1, os.path.join(sys.path[0], '..'))

from pyutils import build_shaders, embed_shaders, make_abs_path

if len(sys.argv) < 4 or not os.path.isdir(sys.argv[1]):
  print('shaders.py <input dir> <tmp dir> <output dir>')
  sys.exit(1)
in_dir  = make_abs_path(sys.argv[1])
tmp_dir = make_abs_path(sys.argv[2])
out_dir = make_abs_path(sys.argv[3])
build_shaders( in_dir, tmp_dir)
embed_shaders(tmp_dir, out_dir, "vk")