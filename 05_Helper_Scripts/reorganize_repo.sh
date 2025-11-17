cd ~/-wigig-dcn-thesis

echo "================================================"
echo "  REORGANIZING WIGIG THESIS REPOSITORY"
echo "================================================"
echo ""

# Create organized folder structure
echo "Creating folder structure..."
mkdir -p 00_Documentation
mkdir -p 01_Simulation_Code
mkdir -p 02_Task1_Beamwidth_Analysis/{Results,Graphs,Scripts}
mkdir -p 03_Task5_Blockage_Failover/{Results,Graphs}
mkdir -p 04_Partial_MCS_Analysis/{Results,Graphs}
mkdir -p 05_Helper_Scripts
mkdir -p 99_Archive

echo "Folder structure created!"
echo ""

# ============================================
# 00. Documentation
# ============================================
echo "Organizing documentation..."

# Move main docs to Documentation folder
[ -f README.md ] && mv README.md 00_Documentation/
[ -f METHODOLOGY_REPORT.md ] && mv METHODOLOGY_REPORT.md 00_Documentation/

# ============================================
# 01. Main Simulation Code
# ============================================
echo "Organizing simulation code..."

# Copy latest code from ns-3
cp ~/ns-allinone-3.40/ns-3.40/scratch/dcn-redundancy-failover.cc 01_Simulation_Code/ 2>/dev/null

# Move other simulation files
[ -f dcn-redundancy-failover.cc ] && mv dcn-redundancy-failover.cc 01_Simulation_Code/ 2>/dev/null
[ -f dcn-hybrid-routing-final.cc ] && mv dcn-hybrid-routing-final.cc 01_Simulation_Code/ 2>/dev/null
[ -f dcn-hybrid-routing.cc ] && mv dcn-hybrid-routing.cc 01_Simulation_Code/ 2>/dev/null
[ -f dcn-4node-modified.cc ] && mv dcn-4node-modified.cc 01_Simulation_Code/ 2>/dev/null

# ============================================
# 02. Task 1 - Beamwidth Analysis
# ============================================
echo "Organizing Task 1 (Beamwidth)..."

# Copy fresh results from ns-3
cp ~/ns-allinone-3.40/ns-3.40/results_sectors*.txt 02_Task1_Beamwidth_Analysis/Results/ 2>/dev/null
cp ~/ns-allinone-3.40/ns-3.40/beamwidth_analysis.csv 02_Task1_Beamwidth_Analysis/Results/ 2>/dev/null

# Copy graphs
cp ~/ns-allinone-3.40/ns-3.40/beamwidth_analysis.png 02_Task1_Beamwidth_Analysis/Graphs/ 2>/dev/null
cp ~/ns-allinone-3.40/ns-3.40/beamwidth_analysis.pdf 02_Task1_Beamwidth_Analysis/Graphs/ 2>/dev/null

# Copy scripts
cp ~/ns-allinone-3.40/ns-3.40/create_beamwidth_csv.py 02_Task1_Beamwidth_Analysis/Scripts/ 2>/dev/null
cp ~/ns-allinone-3.40/ns-3.40/create_beamwidth_graphs.py 02_Task1_Beamwidth_Analysis/Scripts/ 2>/dev/null

# Move any old files
mv results_sectors*.txt 02_Task1_Beamwidth_Analysis/Results/ 2>/dev/null
mv beamwidth_analysis.* 02_Task1_Beamwidth_Analysis/Graphs/ 2>/dev/null
mv create_beamwidth*.py 02_Task1_Beamwidth_Analysis/Scripts/ 2>/dev/null

# ============================================
# 03. Task 5 - Blockage/Failover
# ============================================
echo "Organizing Task 5 (Blockage)..."

# Move graphs
mv wigig_failover_results.png 03_Task5_Blockage_Failover/Graphs/ 2>/dev/null
mv wigig_failover_results.pdf 03_Task5_Blockage_Failover/Graphs/ 2>/dev/null

# Move scripts
mv create_failover_graphs.py 03_Task5_Blockage_Failover/ 2>/dev/null

# ============================================
# 04. Partial MCS Analysis
# ============================================
echo "Organizing MCS Analysis..."

# Copy fresh MCS results from ns-3
cp ~/ns-allinone-3.40/ns-3.40/results_mcs*.txt 04_Partial_MCS_Analysis/Results/ 2>/dev/null
cp ~/ns-allinone-3.40/ns-3.40/wired-wireless-comparison.* 04_Partial_MCS_Analysis/Graphs/ 2>/dev/null

# Move old files
mv results_mcs*.txt 04_Partial_MCS_Analysis/Results/ 2>/dev/null
mv wired-wireless-comparison.* 04_Partial_MCS_Analysis/ 2>/dev/null

# Move scripts
mv create_comparison_graphs.py 04_Partial_MCS_Analysis/ 2>/dev/null
mv create_combined_csv.py 04_Partial_MCS_Analysis/ 2>/dev/null

# ============================================
# 05. Helper Scripts
# ============================================
echo "Organizing helper scripts..."

# Move any remaining Python scripts
mv *.py 05_Helper_Scripts/ 2>/dev/null

# ============================================
# 99. Archive old files
# ============================================
echo "Archiving old files..."

# Move legacy graphs
mv baseline-performance-graph.svg 99_Archive/ 2>/dev/null
mv hybrid-comparison-graph.svg 99_Archive/ 2>/dev/null
mv *.svg 99_Archive/ 2>/dev/null

echo ""
echo "Reorganization complete!"
echo ""

# ============================================
# CREATE README FILES FOR EACH FOLDER
# ============================================

echo "Creating README files..."

# Main README
cat > 00_Documentation/README.md << 'EOF'
# 60 GHz WiGig Data Center Network - Thesis Research

**Student:** L00188373  
**Institution:** ATU - Atlantic Technological University  
**Program:** MSc in Data Analytics  
**Last Updated:** November 17, 2025

## 📋 Repository Structure
```
├── 00_Documentation/           # Project documentation
├── 01_Simulation_Code/         # Main ns-3 simulation programs
├── 02_Task1_Beamwidth_Analysis/    # Beamwidth impact study
├── 03_Task5_Blockage_Failover/     # Link obstruction tests
├── 04_Partial_MCS_Analysis/        # MCS performance comparison
├── 05_Helper_Scripts/              # Utility scripts
└── 99_Archive/                     # Legacy files
```

## ✅ Completed Work (40%)

### Task 1: Beamwidth Analysis
**Finding:** At 1.82m, beamwidth (45°, 22.5°, 11.25°) doesn't affect throughput - all achieve 2,643 Mbps.

### Task 5: Blockage/Failover  
**Finding:** 95% packet loss during obstruction, automatic recovery in <100ms.

### Partial Task 2: MCS Analysis (3/24 complete)
- MCS 1: 249 Mbps
- MCS 12: 2,643 Mbps (88% efficiency - optimal)
- MCS 24: 3,665 Mbps

## 🎯 Remaining Work (60%)

- Complete MCS analysis (21 more tests)
- Distance variation study (0.5m to 10m)
- Hybrid architecture implementation

## 🔗 Links

- **GitHub:** https://github.com/L00188373/-wigig-dcn-thesis
- **Reference:** IEEE 802.11ad-2012 Standard
- **Validation:** Rohde & Schwarz WP 1MA220_0e

---

**For detailed methodology, see:** `METHODOLOGY_REPORT.md`
EOF

# Task 1 README
cat > 02_Task1_Beamwidth_Analysis/README.md << 'EOF'
# Task 1: Beamwidth Analysis

## Objective
Evaluate antenna beamwidth impact on WiGig performance at data center distances.

## Configuration
- MCS: 12 (16-QAM 13/16)
- Distance: 1.82 meters
- Sectors: 8, 16, 32
- Beamwidths: 45°, 22.5°, 11.25°

## Results

| Sectors | Beamwidth | Throughput | Loss |
|---------|-----------|------------|------|
| 8       | 45.00°    | 2,643.31 Mbps | 57.88% |
| 16      | 22.50°    | 2,643.31 Mbps | 57.88% |
| 32      | 11.25°    | 2,643.31 Mbps | 57.88% |

## Key Finding
**No significant throughput variation across beamwidths at short distances.**
Even wider beams provide sufficient gain at 1.82m.

## Files
- `Results/` - Raw simulation outputs
- `Graphs/` - PNG and PDF visualizations
- `Scripts/` - Python analysis code

## Implication
8-sector antennas sufficient for data center deployments, reducing cost.
EOF

# Task 5 README
cat > 03_Task5_Blockage_Failover/README.md << 'EOF'
# Task 5: Simple Blockage with Automatic Failover

## Objective
Demonstrate WiGig sensitivity to obstructions and automatic recovery.

## Test Design
```
Timeline:
├─ 0-5s:   Normal operation
├─ 5-10s:  Link blocked (TX power -50 dBm)
└─ 10-15s: Link recovered
```

## Results
- **Normal throughput:** 2,643 Mbps
- **Obstruction loss:** 95%
- **Recovery time:** <100ms (automatic)
- **Overall loss:** 57.88% (averaged over 15s)

## Key Finding
**WiGig exhibits binary failure mode** - link either works fully or fails completely. No graceful degradation.

## Files
- `Graphs/` - Failover timeline visualization
- `create_failover_graphs.py` - Graph generation script

## Supervisor Feedback
> "The results are interesting... they can also go into your thesis."
EOF

# Task 2/4 README
cat > 04_Partial_MCS_Analysis/README.md << 'EOF'
# Task 2: MCS Performance Analysis (Partial)

## Objective
Analyze throughput vs robustness trade-offs across all 24 MCS levels.

## Status
**3 of 24 complete (12.5%)**

## Completed Tests

| MCS | Modulation | Throughput | Efficiency |
|-----|-----------|------------|-----------|
| 1   | BPSK 1/2  | 249 Mbps   | 64.6% |
| 12  | 16-QAM 13/16 | 2,643 Mbps | 88.0% |
| 24  | 64-QAM 13/16 | 3,665 Mbps | 54.3% |

## Key Finding (Preliminary)
**MCS 12 optimal** - Best balance of throughput and efficiency (88%).

## Remaining Work
Need to test: MCS 2-11, 13-23 (21 tests)

## Expected Graph
Throughput vs MCS showing optimal configuration zone.
EOF

# Simulation Code README
cat > 01_Simulation_Code/README.md << 'EOF'
# WiGig Simulation Programs

## Main Program: dcn-redundancy-failover.cc

**Configurable parameters:**
```bash
--mcs=X          # MCS level (1-24)
--distance=X.X   # Distance in meters
--sectors=X      # Antenna sectors (8, 16, 32)
--simulationTime=X.X  # Duration in seconds
```

**Usage:**
```bash
./dcn-redundancy-failover --mcs=12 --distance=1.82 --sectors=8
```

## Other Programs
- `dcn-hybrid-routing-final.cc` - Hybrid wired/wireless architecture
- `dcn-4node-modified.cc` - Multi-node baseline test

## Building
```bash
cd ~/ns-allinone-3.40/ns-3.40
./ns3 build
./build/scratch/ns3.40-dcn-redundancy-failover-default --mcs=12
```
EOF

echo "README files created!"
echo ""

# ============================================
# CREATE .gitignore
# ============================================

cat > .gitignore << 'EOF'
# NS-3 build files
*.o
*.so
*.a
build/
cmake-cache/

# Python
__pycache__/
*.pyc
*.pyo
.pytest_cache/

# Editor files
*.swp
*.swo
*~
.vscode/
.idea/

# OS files
.DS_Store
Thumbs.db

# Backup files
*.backup
*.bak
*.old
EOF

echo ".gitignore created!"
echo ""

# ============================================
# SHOW NEW STRUCTURE
# ============================================

echo "================================================"
echo "  NEW REPOSITORY STRUCTURE"
echo "================================================"
echo ""

tree -L 2 -F 2>/dev/null || find . -maxdepth 2 -type d | grep -v "\.git" | sort

echo ""
echo "================================================"
echo ""

# ============================================
# GIT OPERATIONS
# ============================================

echo "Adding all changes to git..."
git add .

echo ""
echo "Git status:"
git status

echo ""
read -p "Ready to commit and push? (yes/no): " response

if [ "$response" = "yes" ]; then
    echo ""
    echo "Committing..."
    
    git commit -m "Major reorganization: Clean folder structure with documentation

    - Organized into logical task folders
    - Created README for each section
    - Added .gitignore
    - Updated main documentation
    - All files properly categorized
    
    Structure:
    - 00_Documentation (project docs)
    - 01_Simulation_Code (ns-3 programs)
    - 02_Task1_Beamwidth_Analysis (complete)
    - 03_Task5_Blockage_Failover (complete)
    - 04_Partial_MCS_Analysis (3/24 done)
    - 05_Helper_Scripts (utilities)
    - 99_Archive (legacy files)"
    
    echo ""
    echo "Pushing to GitHub..."
    git push origin main
    
    echo ""
    echo "✅ Repository reorganized and pushed!"
    echo ""
    echo "View at: https://github.com/L00188373/-wigig-dcn-thesis"
else
    echo ""
    echo "Commit cancelled. Review changes with: git status"
fi

echo ""
echo "================================================"
echo "  REORGANIZATION COMPLETE"
echo "================================================"
