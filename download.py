#!/usr/bin/python3

import os
import requests
import shutil
import sys
import time
import zipfile

externalDir = os.path.join(sys.path[0], 'external')
interactive = len(sys.argv) > 1 and sys.argv[1] == 'i'
if interactive:
  compiler = input('Set compiler, empty for default:')
  if compiler:
    compiler = '-G "{}"'.format(compiler)
  buildType = input('Set build type: d for Debug, r for Release, other for default:')
  if buildType == 'd':
    buildType = '-DCMAKE_BUILD_TYPE=Debug'
    buildConfigType = '--config Debug'
  elif buildType == 'r':
    buildType = '-DCMAKE_BUILD_TYPE=Release'
    buildConfigType = '--config Release'
  else:
    buildType = ''
    buildConfigType = ''
else:
  compiler = ''
  buildType = ''
  buildConfigType = ''

def downloadFile(url, filename):
  request = requests.get(url)
  file = open(filename, 'wb')
  file.write(request.content)

def downloadFiles(url, dir, files):
  for file in files:
    downloadFile(url + '/' + file, os.path.join(dir, file))

def downloadSTB():
  print('downloading stb...')
  stbDir = os.path.join(externalDir, 'stb')
  if os.path.isdir(stbDir):
    shutil.rmtree(stbDir)
  os.mkdir(stbDir)
  files = ['stb_image.h', 'LICENSE']
  downloadFiles('https://raw.githubusercontent.com/nothings/stb/master', stbDir, files)

def downloadTinyObj():
  print('downloading tiny obj loader...')
  tinyobjDir = os.path.join(externalDir, 'tinyobj')
  if os.path.isdir(tinyobjDir):
    shutil.rmtree(tinyobjDir)
  os.mkdir(tinyobjDir)
  files = ['tiny_obj_loader.h', 'LICENSE']
  downloadFiles('https://raw.githubusercontent.com/syoyo/tinyobjloader/master', tinyobjDir, files)

def downloadGLM():
  print('downloading glm...')
  archiveName = os.path.join(externalDir, 'glm-master.zip')
  downloadFile('https://github.com/g-truc/glm/archive/master.zip', archiveName)
  print('extracting glm...')
  with zipfile.ZipFile(archiveName, 'r') as archive:
    for file in archive.namelist():
      if file.startswith('glm-master/glm/') and not file.endswith('CMakeLists.txt'):
        archive.extract(file, externalDir)
    archive.extract('glm-master/copying.txt', externalDir)
  os.rename(os.path.join(externalDir, 'glm-master', 'copying.txt'), os.path.join(externalDir, 'glm-master', 'glm', 'LICENSE'))
  glmDir = os.path.join(externalDir, 'glm')
  if os.path.isdir(glmDir):
    shutil.rmtree(glmDir)
  shutil.move(os.path.join(externalDir, 'glm-master', 'glm'), externalDir)
  os.rmdir(os.path.join(externalDir, 'glm-master'))
  os.remove(archiveName)

def downloadGLFW():
  print('downloading glfw...')
  archiveName = os.path.join(externalDir, 'glfw-master.zip')
  downloadFile('https://github.com/glfw/glfw/archive/master.zip', archiveName)
  print('extracting glfw...')
  with zipfile.ZipFile(archiveName, 'r') as archive:
    archive.extractall(externalDir)
  print('building glfw...')
  sourceDir = os.path.join(externalDir, 'glfw-master')
  buildDir = os.path.join(externalDir, 'glfw-build')
  installDir = os.path.join(externalDir, 'glfw')
  if os.path.isdir(installDir):
    shutil.rmtree(installDir)
  os.mkdir(buildDir)
  os.mkdir(installDir)
  cwdOld = os.getcwd()
  os.chdir(buildDir)
  os.system('cmake {} {} -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX={} {}'.format(compiler, buildType, installDir, sourceDir))
  os.mkdir(os.path.join(buildDir, 'docs'))
  os.mkdir(os.path.join(buildDir, 'docs', 'html'))
  os.system('cmake --build . {} --target INSTALL'.format(buildConfigType))
  os.chdir(cwdOld)
  os.remove(archiveName)
  shutil.rmtree(buildDir)
  shutil.rmtree(sourceDir)

def downloadMemAlloc():
  print('downloading vulkan memory allocator...')
  dir = os.path.join(externalDir, 'vk_mem_alloc')
  if os.path.isdir(dir):
    shutil.rmtree(dir)
  os.mkdir(dir)
  url = 'https://raw.githubusercontent.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/master'
  downloadFiles(url, dir, ['LICENSE.txt'])
  downloadFiles(url + '/src', dir, ['vk_mem_alloc.h'])

#downloadSTB()
#downloadTinyObj()
#downloadGLM()
#downloadGLFW()
downloadMemAlloc()
print('complete')