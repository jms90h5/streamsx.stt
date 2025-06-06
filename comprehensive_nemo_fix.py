#!/usr/bin/env python3
"""
Comprehensive fix for NeMo Python 3.9 compatibility
"""
import os
import sys
import re
import glob

def find_and_fix_union_syntax():
    """Find and fix all union syntax issues in NeMo"""
    
    nemo_path = "/homes/jsharpe/.local/lib/python3.9/site-packages/nemo"
    
    # Find all Python files that might have union syntax
    python_files = []
    for root, dirs, files in os.walk(nemo_path):
        for file in files:
            if file.endswith('.py'):
                python_files.append(os.path.join(root, file))
    
    print(f"Found {len(python_files)} Python files to check")
    
    union_pattern = re.compile(r'(\w+)\s*\|\s*(\w+)')
    fixed_files = []
    
    for file_path in python_files:
        try:
            with open(file_path, 'r') as f:
                content = f.read()
            
            # Check if file has union syntax
            if ' | ' in content and ('str |' in content or '| None' in content or '| str' in content):
                print(f"Fixing: {file_path}")
                
                # Backup original
                backup_path = file_path + ".py39backup"
                if not os.path.exists(backup_path):
                    with open(backup_path, 'w') as f:
                        f.write(content)
                
                # Fix union syntax
                lines = content.split('\n')
                
                # Add Union import if needed
                has_union_import = False
                for line in lines:
                    if 'from typing import' in line and 'Union' in line:
                        has_union_import = True
                        break
                    elif 'import typing' in line:
                        has_union_import = True
                        break
                
                if not has_union_import:
                    # Find where to insert Union import
                    for i, line in enumerate(lines):
                        if line.startswith('from typing import'):
                            if 'Union' not in line:
                                lines[i] = line.replace('import ', 'import Union, ')
                            has_union_import = True
                            break
                        elif line.startswith('import typing'):
                            has_union_import = True
                            break
                
                if not has_union_import:
                    # Add new import
                    for i, line in enumerate(lines):
                        if line.startswith('import ') or line.startswith('from '):
                            lines.insert(i, 'from typing import Union')
                            break
                
                # Fix union syntax patterns
                new_content = '\n'.join(lines)
                
                # Common patterns to fix
                patterns = [
                    (r'str\s*\|\s*None', 'Union[str, None]'),
                    (r'str\s*\|\s*Path', 'Union[str, Path]'),
                    (r'int\s*\|\s*None', 'Union[int, None]'),
                    (r'float\s*\|\s*None', 'Union[float, None]'),
                    (r'bool\s*\|\s*None', 'Union[bool, None]'),
                    (r'list\s*\|\s*None', 'Union[list, None]'),
                    (r'dict\s*\|\s*None', 'Union[dict, None]'),
                ]
                
                for pattern, replacement in patterns:
                    new_content = re.sub(pattern, replacement, new_content)
                
                # Write fixed content
                with open(file_path, 'w') as f:
                    f.write(new_content)
                
                fixed_files.append(file_path)
                
        except Exception as e:
            print(f"Error processing {file_path}: {e}")
    
    print(f"Fixed {len(fixed_files)} files")
    return len(fixed_files) > 0

def test_nemo_and_transcribe():
    """Test NeMo and run transcription"""
    try:
        print("Testing NeMo import...")
        
        # Mock youtokentome if still needed
        if 'youtokentome' not in sys.modules:
            class MockYTTM:
                class BPE:
                    def __init__(self, *args, **kwargs):
                        pass
                    def encode(self, text):
                        return [0]
                    def decode(self, ids):
                        return ""
            sys.modules['youtokentome'] = MockYTTM()
        
        import nemo.collections.asr as nemo_asr
        print("✓ NeMo ASR imported successfully")
        
        # Load model
        model_path = "models/nemo_cache_aware_conformer/models--nvidia--stt_en_fastconformer_hybrid_large_streaming_multi/snapshots/ae98143333690bd7ced4bc8ec16769bcb8918374/stt_en_fastconformer_hybrid_large_streaming_multi.nemo"
        
        print(f"Loading model: {model_path}")
        model = nemo_asr.models.EncDecHybridRNNTCTCBPEModel.restore_from(model_path)
        model.eval()
        print("✓ Model loaded successfully")
        
        # Test transcription
        audio_file = "test_data/audio/11-ibm-culture-2min-16k.wav"
        print(f"Transcribing: {audio_file}")
        
        transcriptions = model.transcribe([audio_file])
        
        print(f"\n🎤 REAL NEMO TRANSCRIPTION:")
        print(f"📝 {transcriptions[0]}")
        
        # Save result
        with open("nemo_real_transcription_final.txt", 'w') as f:
            f.write(f"NeMo FastConformer Real Transcription\n")
            f.write(f"Model: stt_en_fastconformer_hybrid_large_streaming_multi\n")
            f.write(f"Audio: {audio_file}\n")
            f.write(f"{'='*60}\n\n")
            f.write(transcriptions[0])
        
        print(f"💾 Saved to: nemo_real_transcription_final.txt")
        return True
        
    except Exception as e:
        print(f"✗ Error: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    print("=== Comprehensive NeMo Python 3.9 Fix ===")
    
    # Fix union syntax issues
    if find_and_fix_union_syntax():
        print("\n=== Testing NeMo transcription ===")
        return test_nemo_and_transcribe()
    else:
        print("No files needed fixing")
        return test_nemo_and_transcribe()

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)