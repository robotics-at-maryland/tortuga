
import os, re, subprocess, shutil

re_package = re.compile(r'Package:\s*([\w\d-]+)')
re_version = re.compile(r'Version:\s*([-_\d\.]+)')
def main():
    base = os.path.dirname(__file__)
    package, version = None, None
    with open(os.path.join(base, 'control')) as control:
        for line in control:
            m = re_package.search(line)
            if m:
                package = m.group(1)
                continue
            m = re_version.search(line)
            if m:
                version = m.group(1)
                continue

    assert package, 'Missing Package tag'
    assert version, 'Missing Version tag'
    name = '%s_%s_i386' % (package, version)
    out_dir = os.path.join(base, name)
    try: os.makedirs(out_dir)
    except: pass
    shutil.copytree('/opt/ram/local', os.path.join(out_dir, 'opt/ram/local'), symlinks=True,
                    ignore=lambda path, files: [os.path.join(path, x) for x in files if not x.endswith('.7z')])

    deb_dir = os.path.join(out_dir, 'DEBIAN')
    try: os.makedirs(deb_dir)
    except: pass
    shutil.copy(os.path.join(base, 'control'), deb_dir)

    # generate md5sums for all of the files
    os.chdir(name)
    with open('DEBIAN/md5sums', 'w') as md5sums:
        for path, dir, files in os.walk('opt/ram/local'):
            for f in files:
                subprocess.call(['md5sum', os.path.relpath(os.path.join(path, f))], stdout=md5sums)
    os.chdir('..')

    subprocess.call(['dpkg', '--build', name])

if __name__ == '__main__':
    main()
