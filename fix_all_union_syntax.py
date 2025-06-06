#!/usr/bin/env python3
"""
Fix ALL union syntax issues in NeMo for Python 3.9 compatibility
"""
import os
import re
import glob

def fix_union_syntax_in_file(file_path):
    """Fix union syntax in a single file"""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        original_content = content
        
        # Add Union import if needed and there are union operators
        if ' | ' in content and 'Union' not in content:
            # Look for existing typing imports
            typing_import_pattern = r'from typing import ([^\n]+)'
            match = re.search(typing_import_pattern, content)
            if match:
                imports = match.group(1)
                if 'Union' not in imports:
                    new_imports = imports + ', Union'
                    content = re.sub(typing_import_pattern, f'from typing import {new_imports}', content)
            else:
                # Add new typing import
                lines = content.split('\n')
                for i, line in enumerate(lines):
                    if line.startswith('import ') or line.startswith('from '):
                        lines.insert(i, 'from typing import Union')
                        break
                content = '\n'.join(lines)
        
        # Fix union syntax patterns
        patterns = [
            # Complex tuple types with union: Tuple[...] | None -> Union[Tuple[...], None]
            (r'(Tuple\[[^\]]+\])\s*\|\s*None', r'Union[\1, None]'),
            # Complex List types with union: List[...] | None -> Union[List[...], None]  
            (r'(List\[[^\]]+\])\s*\|\s*None', r'Union[\1, None]'),
            # Complex Dict types with union: Dict[...] | None -> Union[Dict[...], None]
            (r'(Dict\[[^\]]+\])\s*\|\s*None', r'Union[\1, None]'),
            # Type | None -> Union[Type, None]
            (r'(\w+(?:\[[\w\s,\[\]]+\])?)\s*\|\s*None', r'Union[\1, None]'),
            # Type | Type -> Union[Type, Type]  
            (r'(\w+(?:\[[\w\s,\[\]]+\])?)\s*\|\s*(\w+(?:\[[\w\s,\[\]]+\])?)', r'Union[\1, \2]'),
            # list[Type] -> List[Type]
            (r'\blist\s*\[([^\]]+)\]', r'List[\1]'),
            # dict[K, V] -> Dict[K, V]  
            (r'\bdict\s*\[([^\]]+)\]', r'Dict[\1]'),
            # set[Type] -> Set[Type]
            (r'\bset\s*\[([^\]]+)\]', r'Set[\1]'),
            # tuple[Type] -> Tuple[Type]
            (r'\btuple\s*\[([^\]]+)\]', r'Tuple[\1]'),
        ]
        
        for pattern, replacement in patterns:
            content = re.sub(pattern, replacement, content)
        
        # Add necessary imports if we used List, Dict, etc
        if 'List[' in content and 'List' not in re.search(r'from typing import ([^\n]+)', content or '').group(1) if re.search(r'from typing import ([^\n]+)', content) else True:
            content = re.sub(r'from typing import ([^\n]+)', r'from typing import \1, List', content)
        if 'Dict[' in content and 'Dict' not in re.search(r'from typing import ([^\n]+)', content or '').group(1) if re.search(r'from typing import ([^\n]+)', content) else True:
            content = re.sub(r'from typing import ([^\n]+)', r'from typing import \1, Dict', content)
        if 'Set[' in content and 'Set' not in re.search(r'from typing import ([^\n]+)', content or '').group(1) if re.search(r'from typing import ([^\n]+)', content) else True:
            content = re.sub(r'from typing import ([^\n]+)', r'from typing import \1, Set', content)
        if 'Tuple[' in content and 'Tuple' not in re.search(r'from typing import ([^\n]+)', content or '').group(1) if re.search(r'from typing import ([^\n]+)', content) else True:
            content = re.sub(r'from typing import ([^\n]+)', r'from typing import \1, Tuple', content)
        
        if content != original_content:
            with open(file_path, 'w', encoding='utf-8') as f:
                f.write(content)
            return True
            
    except Exception as e:
        print(f"Error processing {file_path}: {e}")
        
    return False

def main():
    """Fix union syntax in all NeMo files"""
    nemo_path = "/homes/jsharpe/.local/lib/python3.9/site-packages/nemo"
    
    print(f"Scanning {nemo_path} for union syntax issues...")
    
    fixed_count = 0
    for root, dirs, files in os.walk(nemo_path):
        for file in files:
            if file.endswith('.py'):
                file_path = os.path.join(root, file)
                if fix_union_syntax_in_file(file_path):
                    print(f"Fixed: {file_path}")
                    fixed_count += 1
    
    print(f"Fixed {fixed_count} files")

if __name__ == "__main__":
    main()