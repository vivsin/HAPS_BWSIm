/*************************************************************************
 * 
 * CEWiT CONFIDENTIAL
 * __________________
 *
 * All Rights Reserved © 2012 CEWiT, India
 *
 * NOTICE: All information contained herein is, and remains the property of Centre of Excellence in Wireless Technology (CEWiT)
 * and its suppliers, if any. The intellectual and technical concepts contained herein may be proprietary to CEWiT.
 * Unauthorized use, duplication, reverse engineering, any form of redistribution, or use in part or in whole other than by prior,
 * express, printed and signed license for use is strictly forbidden.
 */

#include "../include/Old_OuterLoopLA.h"


OuterLoopLinkAdaptation_S::OuterLoopLinkAdaptation_S(){
  
  isEnabled=false;
  WindowSize=2;
  ServiceNodes.set_length(0);
  WindowCount.set_length(0);
  SuccessWindowCount.set_length(0);
  sinrOffset.set_length(0);
  sinrOffsetWithDelay.set_length(0);
  subframeNumberOffset.set_length(0);
  txNewPackets.set_length(0);
  txNewCorrectPackets.set_length(0);
  LastUpdatedsubframe.set_length(0);
  CQIHistory.set_length(0);
  
  TargetBLER=vec("0.1 0.1");
  
  RateForCQI=vec("0.1523 0.2344 0.377 0.6016 0.8770 1.1758 1.4766 1.9141 2.4063 2.7305 3.3223 3.9023 4.5234 5.1152 5.5547");
  SinrForCQI=vec("-6.5 -4.95 -2.95 -1.05 0.9 2.75 4.75 6.6 8.55 10.4 12.45 14.25 16.1 18.2 20");
  sinrPointsForBLER_10_Percent=vec("-6.5 -6.46296 -6.42956 -6.3925 -6.35902 -6.32188 -6.28824 -6.25096 -6.21711 -6.17962 -6.1455 -6.10776 -6.0733 -6.03525 -6.00041 -5.96198 -5.92671 -5.88786 -5.85209 -5.81278 -5.77648 -5.73665 -5.69977 -5.65938 -5.62188 -5.58091 -5.54276 -5.50115 -5.46232 -5.42007 -5.38052 -5.3376 -5.29732 -5.25371 -5.21269 -5.16838 -5.12659 -5.08151 -5.03903 -4.99346 -4.95 -4.90271 -4.85749 -4.8093 -4.76359 -4.71483 -4.66838 -4.61901 -4.57195 -4.52203 -4.47439 -4.42397 -4.37579 -4.32492 -4.27627 -4.22501 -4.17594 -4.12434 -4.0749 -4.02302 -3.97329 -3.92119 -3.8712 -3.81894 -3.76877 -3.71641 -3.66611 -3.6137 -3.56333 -3.51093 -3.46054 -3.40821 -3.35785 -3.30564 -3.25536 -3.2033 -3.15317 -3.10139 -3.05135 -2.99958 -2.95 -2.89962 -2.85141 -2.80158 -2.75336 -2.70376 -2.65586 -2.60658 -2.55891 -2.50992 -2.46249 -2.41378 -2.36657 -2.31813 -2.27112 -2.22293 -2.1761 -2.12814 -2.08147 -2.03371 -1.98718 -1.93959 -1.89317 -1.84573 -1.79939 -1.75207 -1.70577 -1.65854 -1.61226 -1.56508 -1.51879 -1.47163 -1.4253 -1.37814 -1.33173 -1.28455 -1.23802 -1.1907 -1.14413 -1.09701 -1.05 -1.00145 -0.952952 -0.904007 -0.855647 -0.806696 -0.758108 -0.70906 -0.660362 -0.611249 -0.56244 -0.513279 -0.464377 -0.415187 -0.366211 -0.317013 -0.267983 -0.218797 -0.169736 -0.120584 -0.0715131 -0.0224182 0.0266401 0.0756559 0.124679 0.173594 0.22256 0.271351 0.320239 0.368886 0.417675 0.46616 0.514831 0.563134 0.61167 0.659793 0.708162 0.755993 0.80428 0.85216 0.9 0.946407 0.992828 1.03874 1.08528 1.13107 1.17739 1.22299 1.26922 1.31468 1.36083 1.40618 1.45228 1.49754 1.54362 1.58883 1.63493 1.68011 1.72626 1.77146 1.81769 1.86293 1.90927 1.95459 2.00108 2.04651 2.09317 2.13873 2.1856 2.23133 2.27843 2.32434 2.3717 2.41782 2.46545 2.5118 2.55974 2.6064 2.65458 2.70128 2.75 2.79798 2.84803 2.89648 2.94661 2.99523 3.04571 3.09456 3.14528 3.19432 3.24527 3.29446 3.34561 3.39494 3.44625 3.49567 3.54712 3.59661 3.64816 3.69766 3.74928 3.79877 3.85042 3.89987 3.9515 4.00087 4.05246 4.1017 4.15321 4.20231 4.2537 4.30261 4.35386 4.40255 4.45362 4.50208 4.55293 4.60103 4.65174 4.69986 4.75 4.79636 4.8448 4.89054 4.93904 4.98457 5.03275 5.07799 5.12598 5.17096 5.21877 5.26353 5.31117 5.35574 5.40326 5.44766 5.49508 5.53935 5.58671 5.63088 5.67822 5.72232 5.76965 5.81372 5.8611 5.90517 5.95261 5.99671 6.04425 6.08842 6.13609 6.18035 6.22818 6.27255 6.32057 6.36507 6.41331 6.45805 6.50644 6.55114 6.6 6.6459 6.69602 6.74234 6.79247 6.83889 6.88932 6.93591 6.98652 7.03326 7.08405 7.1309 7.18186 7.2288 7.27989 7.3269 7.3781 7.42516 7.47643 7.5235 7.57483 7.62188 7.67323 7.72023 7.77158 7.8185 7.86981 7.91663 7.96788 8.01457 8.06573 8.11226 8.1633 8.20965 8.26056 8.30673 8.35745 8.40328 8.45394 8.49993 8.55 8.59393 8.642 8.6853 8.7336 8.77681 8.82486 8.86788 8.91585 8.95874 9.00666 9.04945 9.09735 9.14009 9.18801 9.23075 9.27874 9.32151 9.36961 9.41246 9.4607 9.50367 9.55211 9.59524 9.64391 9.68723 9.73617 9.77972 9.82896 9.87278 9.92235 9.96647 10.0164 10.0608 10.1112 10.1559 10.2067 10.2518 10.3029 10.3483 10.4 10.4466 10.4995 10.5466 10.5998 10.6472 10.7008 10.7485 10.8025 10.8505 10.9047 10.953 11.0075 11.056 11.1108 11.1594 11.2144 11.2631 11.3183 11.367 11.4223 11.4711 11.5264 11.5752 11.6305 11.6792 11.7345 11.783 11.8382 11.8866 11.9416 11.9897 12.0445 12.0924 12.1469 12.1945 12.2487 12.2958 12.3498 12.3967 12.45 12.4951 12.5465 12.5908 12.6422 12.6861 12.737 12.7804 12.8308 12.8738 12.9239 12.9664 13.0161 13.0582 13.1075 13.1492 13.1981 13.2395 13.2881 13.3291 13.3774 13.418 13.4661 13.5065 13.5543 13.5944 13.6421 13.6819 13.7294 13.7691 13.8165 13.856 13.9033 13.9427 13.99 14.0294 14.0766 14.1159 14.1633 14.2027 14.25 14.2889 14.3357 14.3745 14.4216 14.4606 14.5079 14.5471 14.5947 14.6341 14.682 14.7218 14.77 14.8101 14.8587 14.8991 14.9482 14.989 15.0386 15.0798 15.1299 15.1716 15.2222 15.2644 15.3155 15.3582 15.4098 15.453 15.5053 15.549 15.6018 15.646 15.6993 15.7441 15.798 15.8432 15.8977 15.9434 15.9984 16.0445 16.1 16.1464 16.2022 16.249 16.3052 16.3524 16.409 16.4566 16.5136 16.5614 16.6187 16.6667 16.7244 16.7726 16.8305 16.8789 16.9369 16.9854 17.0435 17.0921 17.1503 17.1988 17.2571 17.3055 17.3638 17.4121 17.4702 17.5184 17.5763 17.6242 17.682 17.7296 17.7871 17.8344 17.8916 17.9385 17.9953 18.0416 18.0981 18.1443 18.2 18.2442 18.298 18.3414 18.395 18.4379 18.491 18.5333 18.5859 18.6277 18.6798 18.7211 18.7728 18.8136 18.8647 18.905 18.9558 18.9957 19.046 19.0854 19.1354 19.1744 19.2241 19.2627 19.312 19.3503 19.3994 19.4373 19.4862 19.5239 19.5725 19.61 19.6585 19.6957 19.7441 19.7812 19.8295 19.8665 19.9148 19.9519 20 20.0362 20.0834 20.1194 20.1669 20.203 20.2506 20.2869 20.3348 20.3712 20.4194 20.4561 20.5046 20.5416 20.5905 20.6279 20.6772 20.715 20.7647 20.803 20.8533 20.892 20.9428 20.982 21.0334 21.0731 21.1251 21.1654 21.218 21.2589 21.3121 21.3535 21.4073 21.4493 21.5038 21.5463 21.6014 21.6445 21.7001 21.7434");
  ratesPersinrPointsForBLER_10_Percent=vec("0.1523 0.154015 0.155819 0.157536 0.159346 0.161071 0.162892 0.164628 0.166465 0.168216 0.170074 0.171846 0.173728 0.175525 0.177437 0.179264 0.181209 0.183069 0.185053 0.186951 0.188977 0.190918 0.19299 0.194977 0.197101 0.199138 0.201316 0.203408 0.205645 0.207795 0.210094 0.212306 0.214672 0.216949 0.219384 0.22173 0.224239 0.226655 0.229242 0.23174 0.2344 0.236945 0.239655 0.242274 0.245077 0.247782 0.250673 0.253465 0.256449 0.259332 0.262411 0.265387 0.268565 0.271638 0.274916 0.278087 0.281469 0.284742 0.288229 0.291605 0.2952 0.29868 0.302385 0.305971 0.309787 0.31348 0.317408 0.32121 0.325251 0.329162 0.333317 0.337337 0.341606 0.345736 0.35012 0.354359 0.358857 0.363205 0.367818 0.372277 0.377 0.381551 0.38637 0.391023 0.395957 0.400717 0.405759 0.410623 0.415775 0.420742 0.426001 0.431069 0.436434 0.441601 0.44707 0.452334 0.457905 0.463264 0.468936 0.474387 0.480156 0.485698 0.491562 0.497191 0.503147 0.50886 0.514906 0.520702 0.526835 0.532708 0.538925 0.544875 0.551172 0.557194 0.56357 0.569663 0.576111 0.582261 0.58879 0.595033 0.6016 0.607765 0.61426 0.620452 0.627042 0.633302 0.639948 0.646263 0.652976 0.659349 0.666127 0.672557 0.679399 0.685886 0.692793 0.699337 0.706307 0.712907 0.719941 0.726595 0.733692 0.740401 0.747559 0.754321 0.761539 0.768352 0.775629 0.782493 0.789827 0.796738 0.804127 0.811085 0.818526 0.825527 0.833019 0.840059 0.8476 0.854683 0.862262 0.869362 0.877 0.884213 0.89197 0.899227 0.906997 0.914268 0.922069 0.929355 0.937175 0.94447 0.952303 0.959601 0.967443 0.974738 0.982584 0.98987 0.997714 1.00499 1.01282 1.02008 1.0279 1.03514 1.04295 1.05015 1.05794 1.06512 1.07289 1.08004 1.08778 1.09489 1.10261 1.10968 1.11737 1.12441 1.13207 1.13908 1.14671 1.15366 1.16128 1.16826 1.1758 1.18251 1.1898 1.19644 1.20377 1.21041 1.21773 1.22436 1.23169 1.23833 1.24568 1.25234 1.25973 1.26642 1.27386 1.2806 1.28809 1.29489 1.30246 1.30933 1.31698 1.32394 1.33169 1.33874 1.34661 1.35378 1.36177 1.36906 1.3772 1.38462 1.39291 1.40049 1.40894 1.41667 1.4253 1.43321 1.44202 1.45009 1.45911 1.46742 1.4766 1.48491 1.4941 1.50257 1.51202 1.52072 1.53039 1.53931 1.54923 1.55838 1.56854 1.57793 1.58834 1.59797 1.60863 1.61851 1.62943 1.63954 1.65072 1.66108 1.67251 1.68311 1.69478 1.70562 1.71754 1.7286 1.74075 1.75203 1.76441 1.77589 1.78849 1.80016 1.81296 1.82481 1.83779 1.8498 1.86295 1.87513 1.8884 1.90062 1.9141 1.92681 1.94079 1.95366 1.96762 1.9805 1.99453 2.00743 2.02146 2.03433 2.04833 2.06115 2.07509 2.08781 2.10166 2.11426 2.12799 2.14043 2.154 2.16627 2.17965 2.1917 2.20488 2.21669 2.22963 2.24119 2.25387 2.26514 2.27754 2.28851 2.30061 2.31127 2.32307 2.33339 2.34487 2.35488 2.36601 2.37561 2.38649 2.39598 2.4063 2.41439 2.42333 2.43088 2.43976 2.4471 2.45566 2.46275 2.47111 2.47799 2.48619 2.49291 2.50099 2.5076 2.51559 2.52214 2.5301 2.53663 2.5446 2.55116 2.55921 2.56585 2.574 2.58077 2.58909 2.59603 2.60455 2.61172 2.62049 2.62792 2.63699 2.64473 2.65413 2.66221 2.67199 2.68043 2.69063 2.69954 2.71012 2.71927 2.7305 2.74102 2.75364 2.76482 2.77771 2.78929 2.8027 2.81474 2.82859 2.84106 2.85535 2.86823 2.88294 2.89621 2.91131 2.92494 2.94041 2.95438 2.97019 2.98446 3.00058 3.01512 3.03151 3.04629 3.06292 3.0779 3.09472 3.10987 3.12686 3.14213 3.15924 3.1746 3.19179 3.20721 3.22445 3.23989 3.25713 3.27248 3.28977 3.30529 3.3223 3.33672 3.35264 3.36674 3.3828 3.39686 3.41276 3.42669 3.44252 3.45634 3.47207 3.48579 3.50144 3.51505 3.53061 3.54413 3.55961 3.57304 3.58845 3.6018 3.61716 3.63044 3.64575 3.65897 3.67424 3.68742 3.70267 3.71582 3.73107 3.7442 3.75945 3.77259 3.78786 3.80101 3.81632 3.82949 3.84486 3.85809 3.87351 3.88675 3.9023 3.91582 3.93166 3.9453 3.9612 3.9749 3.99091 4.00471 4.02082 4.03471 4.05094 4.06492 4.08126 4.09534 4.11179 4.12596 4.14252 4.15679 4.17346 4.18782 4.2046 4.21905 4.23593 4.25045 4.26743 4.28203 4.29909 4.31376 4.3309 4.34561 4.36282 4.37758 4.39485 4.40964 4.42695 4.44176 4.45909 4.47389 4.49125 4.5061 4.5234 4.53796 4.55498 4.56945 4.5865 4.60093 4.61792 4.63228 4.64923 4.66352 4.6804 4.6946 4.71141 4.72551 4.74223 4.75622 4.77284 4.7867 4.80321 4.81693 4.83332 4.8469 4.86314 4.87656 4.89265 4.9059 4.92184 4.9349 4.95067 4.96354 4.97912 4.99179 5.00719 5.01965 5.03484 5.04709 5.06207 5.07407 5.08886 5.1007 5.1152 5.12649 5.14045 5.15144 5.16526 5.17603 5.18961 5.20016 5.21354 5.22386 5.23704 5.24716 5.26015 5.27007 5.28287 5.2926 5.30523 5.31478 5.32725 5.33663 5.34895 5.35818 5.37037 5.37945 5.39152 5.40047 5.41243 5.42127 5.43314 5.44188 5.45366 5.46233 5.47404 5.48264 5.49431 5.50285 5.51448 5.52297 5.5346 5.54313 5.5547 5.56296 5.57429 5.5825 5.59392 5.60218 5.61363 5.62194 5.63348 5.64186 5.65349 5.66196 5.6737 5.68229 5.69416 5.70287 5.71489 5.72375 5.73593 5.74494 5.7573 5.76649 5.77904 5.78842 5.80118 5.81075 5.82372 5.8335 5.84669 5.85668 5.87011 5.88032 5.89398 5.90443 5.91833 5.92899 5.94314 5.95406 5.96843 5.97943");
  
}

void OuterLoopLinkAdaptation_S::concatTo(OuterLoopLinkAdaptation_S &outerLoopLinkAdaptationInfo){
  
  append(ServiceNodes,outerLoopLinkAdaptationInfo.ServiceNodes);
  
  if(sinrOffset.length()==0)
  {
    txNewPackets=outerLoopLinkAdaptationInfo.txNewPackets;
    txNewCorrectPackets=outerLoopLinkAdaptationInfo.txNewCorrectPackets;
    WindowCount=outerLoopLinkAdaptationInfo.WindowCount;
    SuccessWindowCount=outerLoopLinkAdaptationInfo.SuccessWindowCount;
    sinrOffset=outerLoopLinkAdaptationInfo.sinrOffset;
    sinrOffsetWithDelay=outerLoopLinkAdaptationInfo.sinrOffsetWithDelay;
    subframeNumberOffset=outerLoopLinkAdaptationInfo.subframeNumberOffset;
    CQIHistory=outerLoopLinkAdaptationInfo.CQIHistory;
  }
  else
  {
    append(txNewPackets,outerLoopLinkAdaptationInfo.txNewPackets);
    append(txNewCorrectPackets,outerLoopLinkAdaptationInfo.txNewCorrectPackets);
    append(WindowCount,outerLoopLinkAdaptationInfo.WindowCount);
    append(SuccessWindowCount,outerLoopLinkAdaptationInfo.SuccessWindowCount);
    append(sinrOffset,outerLoopLinkAdaptationInfo.sinrOffset);
    append(sinrOffsetWithDelay,outerLoopLinkAdaptationInfo.sinrOffsetWithDelay);
    append(subframeNumberOffset,outerLoopLinkAdaptationInfo.subframeNumberOffset);
    append(CQIHistory,outerLoopLinkAdaptationInfo.CQIHistory);
  }
  
}

void OuterLoopLinkAdaptation_S::AddTo(ivec ServiceNodeIDs,vec tTargetBLER,vec tRateThresholds,int tWindowSize){
  
  isEnabled = true;
  
  TargetBLER = tTargetBLER;
  rateThresholds=tRateThresholds;
  WindowSize = tWindowSize;
  
  append(ServiceNodes,ServiceNodeIDs);
  
  Array<ivec> WindowCount1;
  int start=WindowCount.length();
  WindowCount1.set_length(ServiceNodeIDs.length());
  append(WindowCount,WindowCount1);
  for(int indx=start;indx<WindowCount.length();indx++)
    WindowCount(indx)=zeros_i(2);
  
  Array<ivec> SuccessWindowCount1;
  start=SuccessWindowCount.length();
  SuccessWindowCount1.set_length(ServiceNodeIDs.length());
  append(SuccessWindowCount,SuccessWindowCount1);
  for(int indx=start;indx<SuccessWindowCount.length();indx++)
    SuccessWindowCount(indx)=zeros_i(2);
  
  Array<vec> sinrOffset1;
  start=sinrOffset.length();
  sinrOffset1.set_length(ServiceNodeIDs.length());
  append(sinrOffset,sinrOffset1);
  for(int indx=start;indx<sinrOffset.length();indx++)
    sinrOffset(indx)=zeros(2);
  
  Array<ivec> txNewPackets1;
  start=txNewPackets.length();
  txNewPackets1.set_length(ServiceNodeIDs.length());
  append(txNewPackets,txNewPackets1);
  for(int indx=start;indx<txNewPackets.length();indx++)
    txNewPackets(indx)=zeros_i(2);
  
  Array<ivec> txNewCorrectPackets1;
  start=txNewCorrectPackets.length();
  txNewCorrectPackets1.set_length(ServiceNodeIDs.length());
  append(txNewCorrectPackets,txNewCorrectPackets1);
  for(int indx=start;indx<txNewCorrectPackets.length();indx++)
    txNewCorrectPackets(indx)=zeros_i(2);
  
  Array<std::queue <vec> > sinrOffsetWithDelay1;
  sinrOffsetWithDelay1.set_length(ServiceNodeIDs.length());
  append(sinrOffsetWithDelay,sinrOffsetWithDelay1);
  
  Array<std::queue <unsigned int> > subframeNumberOffset1;
  subframeNumberOffset1.set_length(ServiceNodeIDs.length());
  append(subframeNumberOffset,subframeNumberOffset1);
  
  Array<std::queue <ivec> > CQIHistory1;
  CQIHistory1.set_length(ServiceNodeIDs.length());
  append(CQIHistory,CQIHistory1);
  
}

void OuterLoopLinkAdaptation_S::AddTo(ivec ServiceNodeIDs){
  
  isEnabled = true;
  append(ServiceNodes,ServiceNodeIDs);
  
  Array<ivec> WindowCount1;
  int start=WindowCount.length();
  WindowCount1.set_length(ServiceNodeIDs.length());
  append(WindowCount,WindowCount1);
  for(int indx=start;indx<WindowCount.length();indx++)
    WindowCount(indx)=zeros_i(2);
  
  Array<ivec> SuccessWindowCount1;
  start=SuccessWindowCount.length();
  SuccessWindowCount1.set_length(ServiceNodeIDs.length());
  append(SuccessWindowCount,SuccessWindowCount1);
  for(int indx=start;indx<SuccessWindowCount.length();indx++)
    SuccessWindowCount(indx)=zeros_i(2);
  
  Array<vec> sinrOffset1;
  start=sinrOffset.length();
  sinrOffset1.set_length(ServiceNodeIDs.length());
  append(sinrOffset,sinrOffset1);
  for(int indx=start;indx<sinrOffset.length();indx++)
    sinrOffset(indx)=zeros(2);
  
  Array<ivec> txNewPackets1;
  start=txNewPackets.length();
  txNewPackets1.set_length(ServiceNodeIDs.length());
  append(txNewPackets,txNewPackets1);
  for(int indx=start;indx<txNewPackets.length();indx++)
    txNewPackets(indx)=zeros_i(2);
  
  Array<ivec> txNewCorrectPackets1;
  start=txNewCorrectPackets.length();
  txNewCorrectPackets1.set_length(ServiceNodeIDs.length());
  append(txNewCorrectPackets,txNewCorrectPackets1);
  for(int indx=start;indx<txNewCorrectPackets.length();indx++)
    txNewCorrectPackets(indx)=zeros_i(2);
  
  Array<std::queue <vec> > sinrOffsetWithDelay1;
  sinrOffsetWithDelay1.set_length(ServiceNodeIDs.length());
  append(sinrOffsetWithDelay,sinrOffsetWithDelay1);
  
  Array<std::queue <unsigned int> > subframeNumberOffset1;
  subframeNumberOffset1.set_length(ServiceNodeIDs.length());
  append(subframeNumberOffset,subframeNumberOffset1);
  
  Array<std::queue <ivec> > CQIHistory1;
  CQIHistory1.set_length(ServiceNodeIDs.length());
  append(CQIHistory,CQIHistory1);
  
}

void OuterLoopLinkAdaptation_S::RemoveTo(int removeIndx){
  
  ServiceNodes.del(removeIndx);
  ivec rmIndx=to_ivec(removeIndx);
  deleteInArray(WindowCount,rmIndx);
  deleteInArray(SuccessWindowCount,rmIndx);
  deleteInArray(sinrOffset,rmIndx);
  deleteInArray(sinrOffsetWithDelay,rmIndx);
  deleteInArray(subframeNumberOffset,rmIndx);
  deleteInArray(txNewPackets,rmIndx);
  deleteInArray(txNewCorrectPackets,rmIndx);
  deleteInArray(CQIHistory,rmIndx);
  
}

void OuterLoopLinkAdaptation_S::clear(int serviceNodeID){

  int indx=find(ServiceNodes,serviceNodeID);
  if(indx!=-1)
  {
    RemoveTo(indx);
    AddTo(to_ivec(serviceNodeID));
  }
}


void OuterLoopLinkAdaptation_S::clear(){
    
  for(int indx=0;indx<sinrOffset.length();indx++)
  {
    txNewPackets(indx).clear();
    txNewCorrectPackets(indx).clear();
    sinrOffset(indx).clear();
    sinrOffsetWithDelay(indx).empty();
    subframeNumberOffset(indx).empty();
    WindowCount(indx).clear();
    SuccessWindowCount(indx).clear();
    CQIHistory(indx).empty();
  }
}

double OuterLoopLinkAdaptation_S::getTargetBLER(double mcsRate){
  
  double targetBLER;
  if(rateThresholds.length())
  {
    vec lesserRates=rateThresholds(find((rateThresholds-mcsRate)<=0));
    return(TargetBLER(lesserRates.length()));
  }
  else
    return(TargetBLER(0));
  
}


void OuterLoopLinkAdaptation_S::update(int nodeID, bvec isSuccess, bvec isNewTransmission, int subframeNumber, double mcsRate){
  
  int indx=find(ServiceNodes,nodeID);
  
  if(indx!=-1) 
  {
    if(sum(to_ivec(isNewTransmission))>0)
    {
      for(int icw=0;icw<isNewTransmission.length();icw++)
      {
	txNewPackets(indx)(icw)+=(int)isNewTransmission(icw);
	if(isNewTransmission(icw))
	  txNewCorrectPackets(indx)(icw)+=(int)isSuccess(icw);
	
	if(WindowCount(indx)(icw)<=WindowSize)
	{
	  WindowCount(indx)(icw)+=(int)isNewTransmission(icw);
	  if(isNewTransmission(icw))
	    SuccessWindowCount(indx)(icw)+=(int)isSuccess(icw);
	}
	
// 	if(WindowCount(indx)(icw)<=WindowSize)
// 	{
// 	  WindowCount(indx)(icw) += 1;
// 	  SuccessWindowCount(indx)(icw) += (int)isSuccess(icw);
// 	}
	
	if(WindowCount(indx)(icw)>=WindowSize)
	{
	  double sinrDown=2.0;
	  double targetBLER=getTargetBLER(mcsRate);
	  double sinrUp=sinrDown/((1.0/targetBLER)-1.0);
	  if(-20.0 < sinrOffset(indx)(icw) && sinrOffset(indx)(icw) < 20.0)
	  {
	    sinrOffset(indx)(icw) +=  ( (double)(SuccessWindowCount(indx)(icw)/(double)WindowCount(indx)(icw)) )*sinrUp - ((WindowCount(indx)(icw)-SuccessWindowCount(indx)(icw))/(double)WindowCount(indx)(icw))*sinrDown;
	  }
	  WindowCount(indx)(icw)=0;
	  SuccessWindowCount(indx)(icw)=0;
	}
      }
    }
  }
  else
  {
    cout<<"[both:] Unknown nodeID to update() in OuterLoopControl_S..."<<endl;
    abort();
  }
  
  if(subframeNumber==0)
  {
    clear();
  }
}

#ifdef USING_L2
void OuterLoopLinkAdaptation_S::updateFromL2(HarqStatInfo tHarqStatInfo){
  
  int nodeID = tHarqStatInfo.rnti;
  if(nodeID!=-1)
  {
#ifdef OLLA_DEBUG
    cout<<"Update OLLA ... "<<endl;
#endif
    bvec isSuccess,isNewTransmission;
    if(tHarqStatInfo.CW1HarqHistory[0].subframe!=-1)
    {
      isSuccess = zeros_b(1);
      isNewTransmission = zeros_b(1);
      isSuccess(0) = tHarqStatInfo.CW1HarqHistory[0].feedback;
      if(tHarqStatInfo.CW1HarqHistory[0].transmissionCount==0)
	isNewTransmission(0) = true;
    }
    if(tHarqStatInfo.CW2HarqHistory.size()!=0 && tHarqStatInfo.CW2HarqHistory[0].subframe!=-1)
    {
      isSuccess.set_length(2,true);
      isSuccess(1) = tHarqStatInfo.CW2HarqHistory[0].feedback;
      isNewTransmission.set_length(2,true);
      if(tHarqStatInfo.CW2HarqHistory[0].transmissionCount==0)
	isNewTransmission(1) = true;
    }
    int subframeNumber = tHarqStatInfo.CW1HarqHistory[0].subframe;
#ifdef OLLA_DEBUG    
    ofstream fout;
    fout.open("Results/OLLAlogs.txt",ios::app);
    fout<<"nodeID : "<<nodeID<<" , isSuccess : "<<isSuccess<<" , isNewTransmission : "<<isNewTransmission<<" , subframeNumber : "<<subframeNumber<<endl;
#endif    
    update(nodeID,isSuccess,isNewTransmission,subframeNumber);
  }
  else
  {
#ifdef OLLA_DEBUG
    cout<<"Skip Update OLLA ... "<<endl;
#endif    
  }
  
}
#endif

vec OuterLoopLinkAdaptation_S::getRateOffset(ivec CQIperCW, int nodeID){
  
  int indx=find(ServiceNodes,nodeID);
  vec rateOffset=zeros(CQIperCW.length());
  if(indx!=-1)
  {
    for(int icw=0;icw<CQIperCW.length();icw++)
    {
      if(CQIperCW(icw)==0)
      {
	cout<<"[both:] CQI 0 is not supported in outerLoopLinkAdaptation.. Aborting "<<endl;abort();
      }

      double sinr=SinrForCQI(CQIperCW(icw)-1);
      sinr+=sinrOffset(indx)(icw);
      int sinrIndx=findClosest(sinrPointsForBLER_10_Percent,sinr);
      rateOffset(icw)=ratesPersinrPointsForBLER_10_Percent(sinrIndx) - RateForCQI(CQIperCW(icw)-1);
    }
    
    if(CQIHistory(indx).size()>4)
    {
      CQIHistory(indx).pop();
    }
    CQIHistory(indx).push(CQIperCW);
    
#ifdef OLLA_DEBUG
    ofstream fout;
    fout.open("Results/OLLAlogs.txt",ios::app);
    if(subframeNumber==-1)
    fout<<"nodeID : "<<nodeID<<" \t CQIperCW : "<<CQIperCW<<" \t subframeNumber : "<<subframeNumber<<" \t sinrOffset : "<<sinrOffset(indx)<<" \t rateOffset : "<<rateOffset<<endl;
#endif

  }
  else
  {
    cout<<"[both:] Unknown nodeID to update() in OuterLoopControl_S..."<<endl;
    abort();
  }
  return rateOffset;
}

vec OuterLoopLinkAdaptation_S::getSINROffsetPerCWIndB(int nodeID){
  
  int indx=find(ServiceNodes,nodeID);
  
  if(indx!=-1)
  {
    return sinrOffset(indx);
  }
  else
  {
    cout<<"[both:] Unknown nodeID to getSINROffsetPerCWIndB() in OuterLoopControl_S..."<<endl;
    abort();
  }
}
