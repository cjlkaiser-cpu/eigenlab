#ifndef PROTEIN_FOLDING_HPP
#define PROTEIN_FOLDING_HPP

#include <vector>
#include <random>
#include <string>

/**
 * ProteinFolding - Simplified HP (Hydrophobic-Polar) lattice protein model
 *
 * Physics:
 * - HP model: sequence of H (hydrophobic) and P (polar) residues
 * - Lattice or off-lattice bead-spring model
 * - Energy: E = sum(contact energies) + spring energy
 * - HH contacts favorable, HP/PP neutral
 * - Simulated annealing or Monte Carlo folding
 *
 * Applications: Understanding protein structure, drug design
 */

enum class ResidueType {
    Hydrophobic,  // H - prefer to be buried
    Polar         // P - prefer to be exposed
};

struct ProteinConfig {
    int sequenceLength = 20;          // Number of residues
    float bondLength = 1.0f;          // Distance between consecutive residues
    float bondStiffness = 100.0f;     // Spring constant for bonds
    float hhContactEnergy = -1.0f;    // Energy for HH contacts (negative = favorable)
    float hpContactEnergy = 0.0f;     // Energy for HP contacts
    float ppContactEnergy = 0.0f;     // Energy for PP contacts
    float contactRadius = 1.5f;       // Distance for contact
    float temperature = 1.0f;         // For Monte Carlo/annealing
    float coolingRate = 0.995f;       // Temperature reduction per step
    float dt = 0.01f;                 // Time step for dynamics
    bool use3D = false;               // 2D or 3D simulation
    float ljEpsilon = 1.0f;           // Lennard-Jones depth
    float ljSigma = 0.8f;             // Lennard-Jones size
};

struct ProteinStats {
    float totalEnergy;
    float bondEnergy;
    float contactEnergy;
    float ljEnergy;
    int numHHContacts;
    int numHPContacts;
    float radiusOfGyration;
    float endToEndDistance;
    float temperature;
    int stepCount;
    bool isFolded;
};

struct Residue {
    float x, y, z;          // Position
    float vx, vy, vz;       // Velocity
    float fx, fy, fz;       // Force
    ResidueType type;
    int index;
};

class ProteinFolding {
public:
    ProteinFolding();
    explicit ProteinFolding(const ProteinConfig& config);

    void init();
    void initFromSequence(const std::string& sequence);  // "HPPHHHPPH..."
    void step();
    void step(int numSteps);

    // Folding methods
    void foldMonteCarlo(int numSteps);       // Monte Carlo with Metropolis
    void foldSimulatedAnnealing(int numSteps);
    void foldMolecularDynamics(int numSteps);

    // Configuration
    void setTemperature(float temp);
    void setCoolingRate(float rate);
    void setContactEnergies(float hh, float hp, float pp);

    // Presets
    void presetAlphaHelix();           // Pattern that forms helix-like
    void presetBetaSheet();            // Pattern that forms sheet-like
    void presetRandomCoil();           // No specific structure
    void presetGlobular();             // Compact spherical fold

    // Data access
    const float* positionData() const { return m_positions.data(); }
    const float* colorData() const { return m_colors.data(); }
    const int* bondIndices() const { return m_bondIndices.data(); }

    int residueCount() const { return static_cast<int>(m_residues.size()); }
    int bondCount() const { return static_cast<int>(m_residues.size()) - 1; }
    ProteinStats getStats() const;
    std::string getSequence() const { return m_sequence; }

private:
    ProteinConfig m_config;
    std::vector<Residue> m_residues;
    std::string m_sequence;

    // Flattened arrays for JS interop
    std::vector<float> m_positions;   // x,y,z per residue
    std::vector<float> m_colors;      // r,g,b per residue
    std::vector<int> m_bondIndices;   // pairs of connected residue indices

    std::mt19937 m_rng;
    int m_stepCount;
    float m_currentTemp;

    void initResidues();
    void generateRandomSequence();
    void updateArrays();

    // Force calculations
    void computeForces();
    float computeBondForce(int i);           // Bond spring force
    float computeContactEnergy() const;      // HP model contact energy
    float computeLJEnergy() const;           // Lennard-Jones non-bonded
    float computeTotalEnergy() const;

    // Monte Carlo
    bool metropolisAccept(float deltaE);
    void perturbResidue(int idx, float amount);

    // Integration
    void velocityVerletStep();

    // Color mapping
    void computeResidueColor(const Residue& r, float& red, float& green, float& blue) const;
};

#endif // PROTEIN_FOLDING_HPP
