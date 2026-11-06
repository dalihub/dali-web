# P3 SW bootstrap 복구 증거

2026-11-06. [결과](../../P3-SW-RECOVERY-RESULTS.md). 이전 [실패 증거](../p3-sw-lifecycle/README.md)는 보존한다.

- runtime-result/verdict/input-hashes: 최종 sw-recovery-003, Firefox 실제9회 정상 렌더(각2프레임), 실패4종·retry 버튼·reload 상한·늦은 controller 검사. exit0.
- headers-*: 직접 헤더 기본 E의 실제2프레임·정상 종료 회귀, exit0.
- validation: 개별 결과 독립 검산, 기존748개·CMake 입력376개 drift0, 최종 검사 코드 hash.

전체 HTTP·driver 로그/프로필은 logs/p3/sw-recovery-003과 sw-recovery-headers-001. 중간001은 WebDriver 전역 const 접근 오류,002는 HTTP cache로 navigation 요청 계수 누락이며 최종 성공 집계에서 제외했다. 테스트 HTML에만 no-store를 추가해 reload 계수를 관측했다.

503 해제 및 의도적으로 잘못 제공한 SW 등록 제거 후 programmatic 버튼 click으로 재시도했다. 제품 bootstrap이 SW를 자동 unregister/repair한다는 뜻이 아니다. Pages/CDN 캐시·장기 수명·Chromium은 검증하지 않았다. 재현은 BUILDING의 SW 절차.
