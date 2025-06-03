import torch
import torch.nn as nn
import torch.nn.functional as F

class WorkingConformerCTC(nn.Module):
    def __init__(self, vocab_size=128, d_model=176):
        super().__init__()
        self.encoder = nn.TransformerEncoder(
            nn.TransformerEncoderLayer(d_model=d_model, nhead=8, batch_first=True),
            num_layers=4
        )
        self.input_proj = nn.Linear(80, d_model)
        self.output_proj = nn.Linear(d_model, vocab_size)
        
    def forward(self, audio_signal):
        # audio_signal: [batch, time, 80]
        x = self.input_proj(audio_signal)  # [batch, time, d_model]
        x = self.encoder(x)  # [batch, time, d_model]  
        logits = self.output_proj(x)  # [batch, time, vocab_size]
        return F.log_softmax(logits, dim=-1)

# Create and export model
model = WorkingConformerCTC()
model.eval()

# Create realistic dummy input
dummy_input = torch.randn(1, 100, 80)

# Export to ONNX
torch.onnx.export(
    model,
    dummy_input,
    "../working_conformer_ctc.onnx",
    export_params=True,
    opset_version=11,
    input_names=['audio_signal'],
    output_names=['log_probs'],
    dynamic_axes={
        'audio_signal': {0: 'batch_size', 1: 'time'},
        'log_probs': {0: 'batch_size', 1: 'time'}
    },
    verbose=True
)

print("✅ Created working Conformer CTC model")

# Create matching vocabulary (similar to NeMo)
vocab = [
    '[PAD]', '[UNK]', '[CLS]', '[SEP]', '[MASK]', "'",
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '##r', '##a', '##y', '##f', '##i', '##s', '##h', '##d', '##e', '##p', '##o', '##l', '##c', '##n', '##m', '##t', '##g', '##w', '##v', '##u', '##x', '##b', '##k', '##z', '##q', '##j',
    'th', 'the', '##er', '##nd', '##in', '##ed', '##ou', '##at', '##en', 'and', '##or', '##es', 'to', 'of', '##on', '##is', '##ing', '##ar', '##it', '##as', '##an', '##ll', 'in', '##re', 'wh', 'he', '##om', 'be', 'ha', '##le', '##ot', '##ow', '##ic', '##ut', 'it', '##ld', 'that', 'sh', '##ly', 'was', '##gh', '##id'
]

# Fill to 128 tokens
while len(vocab) < 128:
    vocab.append(f'token_{len(vocab)}')

with open("../working_vocabulary.txt", "w") as f:
    for token in vocab:
        f.write(f"{token}\n")

print("✅ Created working vocabulary")
