## 1. 선행 연구 조사 및 센서 조사

초반 1~3주차에는 선행 연구 조사 및 센서와 보드 조사를 주로 수행하였다. 이를 통해 본 프로젝트가 나아가야 할 방향성과 적절한 센서와 보드를 선정할 수 있었다. 구제적인 내용은 아래와 같다.

### 1-1. **진동 센서 조사 및 분석**

활동 초기의 가장 주요한 활동은 층간 소음 프로젝트에 사용될 진동 센서의 조사 및 분석이었다. 팀원 곽재원님은 여러 진동 센서의 특징과 장단점, 그리고 제품 구매 링크 등을 상세하게 조사하여 발표하였다. 주요 진동 센서로는 가속도계, 피에조 일렉트릭 센서, 벨로시미터 등이 있었으며, 이들 중에서 감지 능력, 정밀도, 예산, PC 및 Zigbee 통신 가능성 등을 기준으로 후보군을 선정하였다. 특히, MEMS 가속도계 센서 중 ADXL 354 센서가 가장 적합하다는 결론을 내렸다.

홍기현님은 진동을 숫자로 표현하는 척도인 변위, 속도, 가속도의 정의와 변환 공식, 그리고 감지 방식에 따른 센서의 종류와 적합도를 상세하게 분석하였다. 이 중에서도 MEMS 가속도계의 두 가지 주요 기술, 즉 정전용량식과 압저항식(piazo) 중에서는 정전용량식이 이번 프로젝트에 더 적합하다는 분석을 제시하였다. 건축물 진동 모니터링을 위한 가속도계 성분분석 논문을 기반으로, ADXL 354보다 1,000배 민감한 ADXL 355 모델이 더 적합하다는 결론도 도출되었다.

교수님께서는 센서의 감지 범위와 센서가 벽이나 천장에 부착될 때의 작동 안정성에 관한 의문을 제기하셨고, 이에 따른 토의가 활발하게 이루어졌다. 이러한 토론을 통해, 센서의 선택 기준을 더욱 명확히 할 필요성이 있음을 인지하였다.

본 활동을 통해 팀원들 모두 진동 센서의 중요성과 다양한 종류의 센서가 어떻게 작동하는지에 대한 깊은 이해를 얻을 수 있었다.

이 활동에 대한 근거자료로 해당 주제(진동 센서 조사 및 분석)에 대한 내용을 정리한 ‘센서 후보군 조사1.pdf’ 파일을 참고해주시기 바란다.

<br>

### 1-2. **센서 및 보드 조사 및 결정**

"TinyML을 이용한 층간소음 측정" 프로젝트의 주요 구성 요소인 센서와 보드의 선정 또한 주된 활동이었으며 많은 시간을 투자한 활동이었다. 선정 과정에서는 여러 센서와 보드에 대한 데이터시트를 분석하고, 해당 부품들의 적합성을 평가하는 데 큰 노력을 기울였다.

우선, 센서의 경우 MEMS계열 가속도계와 피에조 압전식 압력센서에 대한 데이터시트를 주요하게 분석하였다. 곽재원님의 발표를 통해 AXDL 327 센서가 가장 적합하다는 결론을 내렸으나, 센서의 아날로그 신호 처리 방식에 대한 문제가 발생하였다. 대부분의 MEMS 센서는 가공된 디지털 값만을 제공하는데, 이는 프로젝트의 목표와 맞지 않았다. 따라서 raw data를 직접 얻을 수 있는 방법을 탐색하였고, 이 과정에서 아두이노의 ADC를 활용하여 아날로그 신호를 직접 처리하고, USB를 통해 데이터를 전송하는 방안을 검토했다.

본격적인 구매를 위해 법인카드를 활용하여 TinyML 교재를 구매하였다. 이 교재는 프로젝트의 기반이 될 것으로 예상되며, 머신러닝과 센서 연동에 대한 깊은 이해를 도움이 될 것이다. 홍기현님의 제안으로 MPU 6050 센서의 데이터시트를 확인하였으나, 이 역시 원하는 아날로그 신호를 직접 제공하지 않아 적합하지 않았다. 김근언님은 다양한 머신러닝 보드를 검토하였으나, ADC 신호를 직접 제공하는 보드를 찾는 것이 어려웠다.

팀의 토론을 통해, 아날로그 신호를 직접 제공하는 AXDL 335 센서와 아두이노의 ADC를 함께 활용하는 방안을 결정하였다. 이 방법은 아두이노의 ATMEGA328P의 ADCSRA 레지스터를 활용하여 신호를 바로 TinyML로 연결하는 코드를 작성하는 것이 포함된다.

이 활동을 통해 센서와 보드의 선정 과정에서의 다양한 문제점과 해결 방안을 경험할 수 있었다. 아날로그 신호 처리의 중요성, 센서와 보드의 적합성 평가, 그리고 다양한 부품에 대한 깊은 이해가 필요하다는 것 또한 알아갈 수 있는 중요한 활동이었다.

이 활동에 대한 근거자료로 해당 주제(센서 및 보드 조사 및 결정)에 대한 내용을 정리한 ‘센서 후보군 조사2.pdf’ 파일과 ‘센서 후보군.docx’를 참고해주시기 바란다.

<br>

### 1-3. **선행 프로젝트 공유 및 머신러닝 스터디**

선행 프로젝트 공유 및 머신러닝 스터디는 팀의 주된 활동 중 하나였다. 이 활동은 TinyML이라는 새로운 주제에 대한 이해를 돕기 위해 진행되었다.

김근언 님은 "tenserflow를 활용한 이미지 분류기 제작 프로젝트"에 대해 발표하였다. 이 프로젝트에서는 구글의 오픈소스 API와 데이터 세트를 사용하여 꽃을 인식하고 분류하는 애플리케이션을 개발했다. 김근언 님은 이 프로젝트를 통해 딥러닝과 이미지 분류기의 기본 원리를 학습했으며, 이를 팀원들과 공유했다.

곽재원 님은 "아기 울음소리 분류 프로젝트"를 소개하였다. 이 프로젝트는 아기의 울음소리를 감지하고 원인을 분석하여 청각장애 부모에게 알려주는 시스템을 개발한 것이다. Yamnet과 ResNet50을 활용하여 아기 울음의 원인을 분석하는 모델을 개발했다.

별도로 진행된 머신러닝 스터디에서는 모델링의 기본 원리와 개념을 함께 공부하였다. 교수님의 추천으로 "모두의 딥러닝" 강의를 기반으로 한 스터디가 진행되었다.

이 활동을 통해 팀원들은 선행 프로젝트의 경험과 지식을 공유하며 프로젝트의 진행 방향과 필요 기술에 대한 공감대를 형성할 수 있었다. 머신러닝 스터디는 프로젝트의 성공적인 진행을 위한 기초 지식을 쌓는 데 중요한 역할을 했다.

이 활동에 대한 근거자료로는 ‘뉴럴 네트워크.pdf’ 파일을 제출하였다.

<br>

### 1-4. **Action Plan 및 질의응답**

"TinyML을 이용한 층간소음 측정" 프로젝트에서 Action Plan과 질의응답은 핵심적인 역할을 했다. Action Plan은 프로젝트의 방향성을 설정하고, 각 주차별로 세부 목표와 태스크를 명확히 하는 데 중요했다. 이를 통해 팀원 각각의 역할과 책임이 명확해졌으며, 계획에 따라 활동이 원활히 진행될 수 있었다. 특히, 가속도계 센서의 데이터 수집, TinyML 모델의 훈련 및 코드 분석, 데이터 시각화 등의 작업이 복잡하게 진행될 때, Action Plan을 통해 전체적인 흐름을 이해하고 중요한 결정을 내리는 데 도움이 되었다.

질의응답 세션은 팀원 간의 의견 교환과 문제 해결을 위한 중요한 시간이었다. 코드 분석, 센서 작동 원리, 데이터 분석 결과의 해석 등 다양한 주제에 대한 질문과 답변이 이루어졌다. 이 과정에서 팀원들의 전문 지식과 경험이 공유되었으며, 프로젝트의 전반적인 진행 방향에 대한 공감대가 형성되었다.

이 활동은 프로젝트의 성공적인 진행을 위해 필수적이었다. 팀원 간의 협업 능력 향상에 기여했으며, 프로젝트 목표를 효과적으로 달성하는 데 중요한 역할을 했다. 이 경험은 앞으로의 연구와 프로젝트에서도 유용하게 활용될 것으로 기대된다.

근거자료로는 Action Plan의 캡쳐 이미지가 제출되었다.

<img src="https://raw.githubusercontent.com/jaewonE/define_floor_noise/main/images/action_plan.png" alt="Action plan"/>
