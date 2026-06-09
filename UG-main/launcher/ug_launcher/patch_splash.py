#!/usr/bin/env python3
"""
UG Splash Patcher - Zamjenjuje SA-MP splash bitmapu u samp.dll sa UG splash bitmapom.

Koristi pure Python (bez Resource Hackera) - trazi BMP resource u samp.dll
i zamjenjuje ga sa ug_splash.bmp.

Upotreba:
  python patch_splash.py [samp.dll_path] [ug_splash.bmp_path] [output.dll_path]

Default:
  python patch_splash.py
  - cita samp.dll i ug_splash.bmp iz trenutnog foldera
  - pise ug_samp.dll u trenutni folder
"""

import struct
import sys
import os

def find_bmp_resource_offset(data):
    """
    Pronadji offset i velicinu BMP resursa u samp.dll.
    SA-MP splash bitmapa je obicno resource tipa BITMAP sa ID 128.
    Pretrazujemo PE resource section za BMP header signaturu.
    """
    # PE signature
    if data[:2] != b'MZ':
        print("[ERROR] Nije validan PE fajl (nema MZ signature)")
        return None, None
    
    pe_offset = struct.unpack_from('<I', data, 0x3C)[0]
    if data[pe_offset:pe_offset+4] != b'PE\x00\x00':
        print("[ERROR] Nije validan PE fajl (nema PE signature)")
        return None, None
    
    # Parse PE headers to find resource section
    num_sections = struct.unpack_from('<H', data, pe_offset + 6)[0]
    opt_header_size = struct.unpack_from('<H', data, pe_offset + 20)[0]
    sections_offset = pe_offset + 24 + opt_header_size
    
    resource_section = None
    for i in range(num_sections):
        sec_offset = sections_offset + i * 40
        sec_name = data[sec_offset:sec_offset+8].rstrip(b'\x00').decode('ascii', errors='replace')
        sec_vsize = struct.unpack_from('<I', data, sec_offset + 8)[0]
        sec_rva = struct.unpack_from('<I', data, sec_offset + 12)[0]
        sec_rawsize = struct.unpack_from('<I', data, sec_offset + 16)[0]
        sec_rawptr = struct.unpack_from('<I', data, sec_offset + 20)[0]
        
        if sec_name == '.rsrc':
            resource_section = {
                'rva': sec_rva,
                'raw_ptr': sec_rawptr,
                'raw_size': sec_rawsize,
                'vsize': sec_vsize
            }
            print(f"[OK] Resource section nadjena: RVA=0x{sec_rva:X}, RawPtr=0x{sec_rawptr:X}, Size={sec_rawsize}")
            break
    
    if not resource_section:
        print("[ERROR] Nije nadjena .rsrc sekcija u DLL-u")
        return None, None
    
    # Search for BMP data within resource section
    # SA-MP splash is typically 573x421 24-bit BMP (about 722KB)
    # BMP signature is 'BM' followed by file size
    rsrc_start = resource_section['raw_ptr']
    rsrc_end = rsrc_start + resource_section['raw_size']
    
    # Find all BMP signatures in resource section
    bmp_locations = []
    pos = rsrc_start
    while pos < rsrc_end - 6:
        if data[pos:pos+2] == b'BM':
            bmp_size = struct.unpack_from('<I', data, pos + 2)[0]
            bmp_width = struct.unpack_from('<i', data, pos + 18)[0]
            bmp_height = struct.unpack_from('<i', data, pos + 22)[0]
            bmp_bpp = struct.unpack_from('<H', data, pos + 28)[0]
            
            if bmp_width > 100 and bmp_height > 100 and bmp_size > 10000:
                bmp_locations.append({
                    'offset': pos,
                    'size': bmp_size,
                    'width': bmp_width,
                    'height': bmp_height,
                    'bpp': bmp_bpp
                })
                print(f"  [BMP] Offset=0x{pos:X}, Size={bmp_size}, {bmp_width}x{bmp_height} {bmp_bpp}-bit")
        pos += 1
    
    if not bmp_locations:
        print("[ERROR] Nije nadjena BMP resursa u .rsrc sekciji")
        return None, None
    
    # SA-MP splash je obicno najveci BMP (ili jedini veliki)
    # Sortiraj po velicini, uzmi najveci
    bmp_locations.sort(key=lambda x: x['size'], reverse=True)
    splash_bmp = bmp_locations[0]
    print(f"[OK] Identifikovana splash bitmapa: {splash_bmp['width']}x{splash_bmp['height']} {splash_bmp['bpp']}-bit, size={splash_bmp['size']}")
    
    return splash_bmp['offset'], splash_bmp['size']


def patch_splash_dll(samp_dll_path, splash_bmp_path, output_path):
    """Patch samp.dll sa novom splash bitmapom."""
    
    # Citaj samp.dll
    print(f"[1/4] Citam {samp_dll_path}...")
    with open(samp_dll_path, 'rb') as f:
        dll_data = bytearray(f.read())
    print(f"  Velicina: {len(dll_data)} bytes")
    
    # Citaj UG splash BMP
    print(f"[2/4] Citam {splash_bmp_path}...")
    with open(splash_bmp_path, 'rb') as f:
        new_bmp_data = f.read()
    print(f"  Velicina: {len(new_bmp_data)} bytes")
    
    # Verify new BMP is valid
    if new_bmp_data[:2] != b'BM':
        print("[ERROR] ug_splash.bmp nije validan BMP fajl!")
        return False
    
    # Pronadji originalnu splash bitmapu
    print("[3/4] Trazenim splash bitmapu u samp.dll...")
    bmp_offset, bmp_size = find_bmp_resource_offset(dll_data)
    
    if bmp_offset is None:
        print("[ERROR] Ne mogu pronaci splash bitmapu u samp.dll")
        return False
    
    # Provjeri velicinu - nova BMP mora biti JEDNAKA ili MANJA od originalne
    if len(new_bmp_data) > bmp_size:
        print(f"[WARNING] Nova BMP ({len(new_bmp_data)} bytes) je veca od originalne ({bmp_size} bytes)")
        print("[WARNING] Zamjena mozda nece raditi - koristi manju bitmapu!")
        print(f"[INFO] Preporučena veličina: ista kao originalna ({bmp_size} bytes)")
        # Still try - we'll pad with zeros if smaller, or truncate if larger
        if len(new_bmp_data) > bmp_size + 1024:
            print("[ERROR] Nova BMP je previse velika - prekidam!")
            return False
    
    # Zamijeni BMP podatke u DLL-u
    # Ako je nova BMP manja, ostatak popunimo nulama
    replacement_size = min(len(new_bmp_data), bmp_size)
    dll_data[bmp_offset:bmp_offset + replacement_size] = new_bmp_data[:replacement_size]
    
    # Ako je nova BMP manja, nuliraj ostatak
    if len(new_bmp_data) < bmp_size:
        remaining = bmp_size - len(new_bmp_data)
        dll_data[bmp_offset + len(new_bmp_data):bmp_offset + bmp_size] = b'\x00' * remaining
        print(f"  [INFO] Popunjeno {remaining} bytes nulama (nova BMP je manja)")
    
    # Zapisi patchovani DLL
    print(f"[4/4] Pisem {output_path}...")
    with open(output_path, 'wb') as f:
        f.write(dll_data)
    
    output_size = os.path.getsize(output_path)
    print(f"\n{'='*50}")
    print(f"  GOTOVO! ug_samp.dll kreiran ({output_size} bytes)")
    print(f"{'='*50}")
    print(f"\nLauncher ce automatski instalirati ug_samp.dll kao samp.dll")
    print(f"u GTA San Andreas folder za sve igrace.")
    
    return True


if __name__ == '__main__':
    samp_dll = sys.argv[1] if len(sys.argv) > 1 else 'samp.dll'
    splash_bmp = sys.argv[2] if len(sys.argv) > 2 else 'ug_splash.bmp'
    output_dll = sys.argv[3] if len(sys.argv) > 3 else 'ug_samp.dll'
    
    # Check files exist
    if not os.path.exists(samp_dll):
        print(f"[ERROR] {samp_dll} nije pronadjen!")
        sys.exit(1)
    if not os.path.exists(splash_bmp):
        print(f"[ERROR] {splash_bmp} nije pronadjen!")
        sys.exit(1)
    
    success = patch_splash_dll(samp_dll, splash_bmp, output_dll)
    sys.exit(0 if success else 1)
