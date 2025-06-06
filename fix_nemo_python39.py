#!/usr/bin/env python3
"""
Fix NeMo Python 3.9 compatibility and run real transcription
"""
import sys
import os
from pathlib import Path

def patch_nemo_for_python39():
    """Patch NeMo files to be compatible with Python 3.9"""
    
    # Find the problematic file
    nemo_path = "/homes/jsharpe/.local/lib/python3.9/site-packages/nemo"
    problematic_file = os.path.join(nemo_path, "collections/common/tokenizers/canary_tokenizer.py")
    
    if os.path.exists(problematic_file):
        print(f"Found problematic file: {problematic_file}")
        
        # Read the file
        with open(problematic_file, 'r') as f:
            content = f.read()
        
        # Check if it needs patching
        if 'str | Path' in content:
            print("Patching union type syntax...")
            
            # Add typing import at the top
            lines = content.split('\n')
            import_line_idx = -1
            for i, line in enumerate(lines):
                if line.startswith('from typing import') or line.startswith('import typing'):
                    import_line_idx = i
                    break
            
            if import_line_idx >= 0:
                # Add Union to existing typing import
                if 'Union' not in lines[import_line_idx]:
                    lines[import_line_idx] = lines[import_line_idx].replace('import ', 'import Union, ')
            else:
                # Add new typing import
                for i, line in enumerate(lines):
                    if line.startswith('import ') or line.startswith('from '):
                        lines.insert(i, 'from typing import Union')
                        break
            
            # Replace union syntax
            content = '\n'.join(lines)
            content = content.replace('str | Path', 'Union[str, Path]')
            
            # Backup original and write patched version
            backup_file = problematic_file + '.backup'
            if not os.path.exists(backup_file):
                os.rename(problematic_file, backup_file)
                print(f"Backed up original to: {backup_file}")
            
            with open(problematic_file, 'w') as f:
                f.write(content)
            
            print("✓ File patched successfully")
            return True
        else:
            print("File doesn't need patching")
            return True
    else:
        print(f"Problematic file not found: {problematic_file}")
        return False

def test_nemo_import():
    """Test if NeMo ASR can be imported after patching"""
    try:
        import nemo.collections.asr as nemo_asr
        print("✓ NeMo ASR imported successfully")
        return True
    except Exception as e:
        print(f"✗ NeMo ASR import failed: {e}")
        return False

def run_real_nemo_transcription():
    """Run real NeMo transcription"""
    try:
        import nemo.collections.asr as nemo_asr
        
        print("=== Real NeMo FastConformer Transcription ===")
        
        # Load the model
        model_path = "models/nemo_cache_aware_conformer/models--nvidia--stt_en_fastconformer_hybrid_large_streaming_multi/snapshots/ae98143333690bd7ced4bc8ec16769bcb8918374/stt_en_fastconformer_hybrid_large_streaming_multi.nemo"
        
        print(f"Loading model: {model_path}")
        model = nemo_asr.models.EncDecHybridRNNTCTCBPEModel.restore_from(model_path)
        model.eval()
        
        print("✓ Model loaded successfully")
        print(f"Model type: {type(model)}")
        
        # Test audio file
        audio_file = "test_data/audio/11-ibm-culture-2min-16k.wav"
        print(f"\nTranscribing: {audio_file}")
        
        # Transcribe the audio
        transcription = model.transcribe([audio_file])
        
        print(f"\n🎤 REAL NEMO TRANSCRIPTION:")
        print(f"📝 {transcription[0]}")
        
        # Save transcription
        output_file = "nemo_real_transcription.txt"
        with open(output_file, 'w') as f:
            f.write(f"NeMo FastConformer Real Transcription\n")
            f.write(f"Model: stt_en_fastconformer_hybrid_large_streaming_multi\n")
            f.write(f"Audio: {audio_file}\n")
            f.write(f"{'='*60}\n\n")
            f.write(transcription[0])
        
        print(f"💾 Saved to: {output_file}")
        
        return True
        
    except Exception as e:
        print(f"✗ Transcription failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    print("=== Fixing NeMo for Python 3.9 ===")
    
    # Try patching
    if patch_nemo_for_python39():
        print("\n=== Testing NeMo Import ===")
        if test_nemo_import():
            print("\n=== Running Real Transcription ===")
            return run_real_nemo_transcription()
        else:
            print("❌ Import test failed")
            return False
    else:
        print("❌ Patching failed")
        return False

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)