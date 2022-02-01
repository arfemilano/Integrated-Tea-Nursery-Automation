#include <Neurona.h>
#define NET_INPUTS 2
#define NET_OUTPUTS 1
#define NET_LAYERS 10
int layerSizes[] = {10, NET_OUTPUTS, -1};
double input[] = {0, 0, 0}; //RGB values
double netInput[] = { -1.0, 0.0, 0.0, 0.0};
double PROGMEM const initW[] = {0.6713582310115045, 0.21203305159507896, 0.5587391931448817, 0.9372873734373388, -0.035608110849273185, 0.07834218244643541, -0.1959584798685783, 0.6402928655135717, 0.11170965964565051, 0.23039595696321652, 0.3967788788845199, 0.1370185565929976, 0.4146349663075753, 0.11850771597617916, 0.25472909547567485, 0.23287612961007295, 0.3350347169991384, 0.41482710124576816, 1.1360317362699366, 0.1915387772254049, 0.24856998304718064, 0.6715534465028948, 0.5277225955698985, 0.05666121098311258, 0.3535709580734592, 0.3024113829794287, 0.5867235162056331, 1.0921250882361693, 0.0012633689933778118, 0.5195636080818201, 2.5577353045001625, -0.7093078270943446, 0.22294835644038882, -1.2207254201759188, -0.3502374017720177, -0.1342565676413586, -0.3655050614918511, 0.318468617060471, -0.7504144956202001, -0.615898895817716, 0.2249156939237123};
MLP mlp(NET_INPUTS, NET_OUTPUTS, layerSizes, MLP::LOGISTIC, initW, true);
double temp, humid;
int level;
void setup() {
  //Inisialisasi Serial Monitor
  Serial.begin(115200);
  while (!Serial);

}

void loop() {
  input[0] = temp / 100;
  input[1] = humid / 100;
  for (int k = 0; k < 3; k++)
  {
    if (k < 2) {
      netInput[k + 1] = (double)input[k];
    }
  }
  double *ANNprediction = mlp.forward(netInput);
  delay(3000);
  level = (double)(*ANNprediction);
  Serial.print(level);
  //  if (level < 0.16)
  //  {
  //    hama = "Tidak Berpotensi";
  //  }
  //  else {
  //    hama = "Berpotensi";
  //  }
}