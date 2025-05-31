# Documentation Index - Teracloud STT Toolkit

## 🚨 START HERE: Critical Reference Documents

### **NEMO_CACHE_AWARE_GUIDE.md** 
**MOST IMPORTANT** - Complete guide for NeMo cache-aware streaming models
- The problem we solved (repeated "sh" output)
- How to export models with cache support
- Verification commands
- Troubleshooting guide
- **Never delete this file**

### **QUICK_REFERENCE.md**
Fast fixes and one-line solutions for NeMo issues

## 📊 Status Documents (Current Reality)

### **REAL_MODEL_SUCCESS.md** ✅ ACCURATE
Documents the actually working wav2vec2 model with real transcription results

### **NEMO_INTEGRATION_STATUS.md** ❌ INCORRECT  
Claims NeMo model works but provides no proof. Ignore this document.

### **NEMO_WORK_PLAN_UPDATE.md**
Investigation notes and test results from May 31, 2025

## 🔧 Working Scripts

### **download_and_export_fresh.py** 
Complete solution to download and export NeMo model with cache support

### **test_fixed_model.py**
Verify exported models have correct cache tensor structure

### **fix_nemo_export_final.py**
Alternative export method if download script fails

## 📋 Planning Documents

### **NEMO_WORK_PLAN.md**
Original planning document with phases and objectives

### **ARCHITECTURE.md** 
High-level toolkit architecture overview

### **README_MODELS.md**
Guide to different model types supported by toolkit

## 🏗️ Technical Implementation

### **TRANSCRIPTION_FIX_PLAN.md**
Technical details about transcription pipeline fixes

### **FIX_NEMO_MODEL.md** 
Specific NeMo model integration issues and solutions

## 📈 Historical Context

### **NEMO_DEBUG_PLAN.md**
Debugging approach for NeMo integration issues

### **NEMO_PYTHON_DEPENDENCIES.md**
Python dependency management for NeMo toolkit

## 🚀 Current Working State

**What Actually Works:**
- wav2vec2_base.onnx (361MB) - Character-level transcription
- C++ ONNX Runtime integration
- Feature extraction pipeline

**What Needs Fixing:**
- NeMo FastConformer model (needs cache-aware export)
- Streaming inference with proper state management

**Next Steps:**
1. Run `python3 download_and_export_fresh.py`
2. Test with cache-aware model
3. Verify streaming transcription works

---

**🔥 Remember**: The key to NeMo models is `model.set_export_config({'cache_support': 'True'})` before export