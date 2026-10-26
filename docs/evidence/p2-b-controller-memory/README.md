# 생산 Buffer/Controller CPU 통합 검사 중간 기록

2026-10-23. `controller-005`: 생산 33개 객체 재컴파일/nm, 테스트 링크 및 Node 12모드 각각 exit 0. 실제 Controller/Buffer factory와 Core UniformBufferV2를 사용하고 GL 응답을 주입한다. GPU·브라우저·생산 strict 링크 성공 증거가 아니다.

`commands.json`은 명령·개별 종료 코드, `source-hashes.json`은 입력, `results.txt`는 결과다. Core archive는 memory-web-003을 사용했다. 테스트 전용 GL abstraction은 예상하지 않은 호출을 abort하며 shader binary 파일 접근도 abort한다. 이 테스트 구현은 생산 target에 포함하지 않는다.

CreateBuffer allocation 및 Buffer/Storage/Memory nothrow 객체 할당 실패, GL buffer 생성/BufferData 실패, 실제 same-size recycle/grow, create 전 discard, Core CPU/GPU uniform, live Memory shutdown, 후속 command 제출 차단을 검사했다. `controller-004`는 shutdown 후 custom allocator 객체 해제 callback 누락을 재현했다. 공통 GLES deleter와 SyncDeleter를 수정하고 불완전한 allocation callback 쌍을 factory에서 거부했다.

전체 로그/산출물은 logs/p2-b/controller-001~005 및 build/p2-b/에 보존한다. 001/002 테스트 compile 실패, 003 미설정 GL 응답 abort, 004 allocator 실패도 보존한다. host failure/stop, wrapper/cache 및 생산 링크는 진행 중이다. native 회귀는 최종 공통 변경 후 수행한다. P3 GPU 보존·D3/D4·P3/P4 게이트 유지.
