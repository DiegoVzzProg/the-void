import sys
import re
import random
import os

# --- CONFIGURACIÓN ---
# Lista AQUÍ todos los archivos donde usas OBF()
target_files = [
    "src/modules/LeftAutoClicker.h",
    # "src/modules/RightClicker.h",  <-- Agrega más aquí cuando crees más módulos
    # "src/modules/AimAssist.cpp"
]

output_header = "src/sdk/obf_generated.h"

# ---------------------

def get_storage_type(t_type):
    """Determina el tipo de dato para guardar los bits ofuscados"""
    if "float" in t_type: return "uint32_t"
    if "double" in t_type or "long long" in t_type: return "uint64_t"
    return "uint32_t" # Default para int, bool, etc.

def generate_specialization(t_type, id_val):
    """Genera código C++ con matemáticas aleatorias XOR/ADD/SUB"""
    
    # Generar llaves aleatorias
    xor_key = hex(random.randint(0x1000, 0xFFFFFFFF))
    add_key = hex(random.randint(0x1000, 0xFFFFFFFF))
    
    storage = get_storage_type(t_type)
    is_floating = "float" in t_type or "double" in t_type
    
    # Lógica de codificación (Encode) y decodificación (Decode)
    if is_floating:
        # Para floats usamos UNION para tratar sus bits como enteros
        encode_code = f"""
            union {{ {t_type} f; {storage} i; }} u;
            u.f = v;
            val_ = (u.i ^ {xor_key}) + {add_key};
        """
        decode_code = f"""
            union {{ {t_type} f; {storage} i; }} u;
            u.i = (val_ - {add_key}) ^ {xor_key};
            return u.f;
        """
    else:
        # Para enteros es matemática directa
        # (v ^ XOR) + ADD
        encode_code = f"val_ = ({storage}(v) ^ {xor_key}) + {add_key};"
        decode_code = f"return (val_ - {add_key}) ^ {xor_key};"

    # Plantilla C++
    return f"""
// ID: {id_val} | Tipo: {t_type}
template<>
class obf<{t_type}, {id_val}> {{
private:
    {storage} val_;
public:
    FORCE_INLINE obf() : val_(0) {{}}
    FORCE_INLINE obf({t_type} v) {{ encrypt(v); }}

    FORCE_INLINE void encrypt({t_type} v) {{
        {encode_code}
    }}

    FORCE_INLINE {t_type} decrypt() const {{
        {decode_code}
    }}

    FORCE_INLINE operator {t_type}() const {{ return decrypt(); }}
    FORCE_INLINE obf& operator=({t_type} v) {{ encrypt(v); return *this; }}
    
    // Operadores matemáticos mapeados
    FORCE_INLINE obf& operator+=({t_type} v) {{ encrypt(decrypt() + v); return *this; }}
    FORCE_INLINE obf& operator-=({t_type} v) {{ encrypt(decrypt() - v); return *this; }}
    FORCE_INLINE obf& operator*=({t_type} v) {{ encrypt(decrypt() * v); return *this; }}
    FORCE_INLINE obf& operator/=({t_type} v) {{ encrypt(decrypt() / v); return *this; }}
}};
"""

def main():
    print("[POLYMORPH] Iniciando generacion de codigo basura...")
    
    all_matches = []
    
    # 1. Escanear archivos
    for filepath in target_files:
        if not os.path.exists(filepath):
            print(f"[ERROR] No se encuentra: {filepath}")
            continue
            
        with open(filepath, 'r') as f:
            content = f.read()
            # Regex mejorado: Captura OBF( int ) o OBF(long long)
            matches = re.findall(r'OBF\s*\(\s*(.*?)\s*\)', content)
            all_matches.extend(matches)

    # 2. Generar Header
    try:
        with open(output_header, 'w') as out:
            out.write("#pragma once\n// GENERADO AUTOMATICAMENTE - NO EDITAR\n")
            out.write(f"// Generado con semilla: {random.randint(0,999999)}\n\n")
            
            counter = 0
            for type_name in all_matches:
                # Limpiar espacios extra
                clean_type = " ".join(type_name.split())
                out.write(generate_specialization(clean_type, counter))
                counter += 1
                
        print(f"[EXITO] {counter} variables ofuscadas generadas en {output_header}")
        print("[INFO] Recompila tu proyecto C++ ahora.")
        
    except Exception as e:
        print(f"[ERROR] Al escribir archivo: {e}")

if __name__ == "__main__":
    main()