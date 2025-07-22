; void AnmVm::run() -> run(AnmVm* This)
  44b4b0:	55                   	push   %ebp
  44b4b1:	8b ec                	mov    %esp,%ebp
  44b4b3:	83 e4 f8             	and    $0xfffffff8,%esp
  44b4b6:	81 ec cc 00 00 00    	sub    $0xcc,%esp ; stack frame
  44b4bc:	53                   	push   %ebx
  44b4bd:	56                   	push   %esi
  44b4be:	57                   	push   %edi
  44b4bf:	8b 7d 08             	mov    0x8(%ebp),%edi ; This--offset 0x8 hints that there's another param?
  44b4c2:	83 bf a8 03 00 00 00 	cmpl   $0x0,0x3a8(%edi) ; if (!This->currentInstruction)
  44b4c9:	0f 84 f5 04 00 00    	je     0x44b9c4 ; Exit
  44b4cf:	8b 87 04 04 00 00    	mov    0x404(%edi),%eax ; flagsLow
  44b4d5:	a9 00 00 02 00       	test   $0x20000,%eax ; flagsLow == 0x20000
  44b4da:	0f 85 3d 2d 00 00    	jne    0x44e21d ;Exit
  44b4e0:	d9 05 48 79 4a 00    	flds   0x4a7948 ; g_gameSpeed;
  44b4e6:	d9 5c 24 74          	fstps  0x74(%esp) ; gameSpeed = g_gameSpeed;
  44b4ea:	a9 00 00 00 20       	test   $0x20000000,%eax ; if ((This->flagsLow & 0x20000000) != 0)
  44b4ef:	74 08                	je     0x44b4f9
  44b4f1:	d9 e8                	fld1 ; 1.0
  44b4f3:	d9 1d 48 79 4a 00    	fstps  0x4a7948 ; g_gameSpeed
  44b4f9:	0f b7 8f 7c 03 00 00 	movzwl 0x37c(%edi),%ecx
  44b500:	66 85 c9             	test   %cx,%cx ; pendingInterrupt
  44b503:	0f 85 aa 00 00 00    	jne    0x44b5b3 ; Interrupt


; struct AnmRawInstruction
; {
;     uint16_t opcode;
;     uint16_t offsetToNextInstr;
;     short time;
;     uint16_t varMask;
;     uint32_t args[10];
; };
; Loop:
  44b509:	8b 9f a8 03 00 00    	mov    0x3a8(%edi),%ebx ; This->currentInstruction
  44b50f:	0f bf 43 04          	movswl 0x4(%ebx),%eax ; This->currentInstruction->time
  44b513:	3b 47 60             	cmp    0x60(%edi),%eax ; if (currentInstruction->time <= timeInScript.m_current)
  44b516:	89 5c 24 18          	mov    %ebx,0x18(%esp) ; local var AnmRawInstruction* instr = currentInstruction;
  44b51a:	0f 8f 8e 0d 00 00    	jg     0x44c2ae ; Skip opcode execution
  44b520:	0f bf 03             	movswl (%ebx),%eax
  44b523:	40                   	inc    %eax ; uint16_t opcode = instr->opcode;
  44b524:	83 f8 67             	cmp    $0x67,%eax ; switch-statement bounds check
  44b527:	0f 87 e7 22 00 00    	ja     0x44d814
  44b52d:	ff 24 85 28 e2 44 00 	jmp    *0x44e228(,%eax,4) ; switch (opcode)
  44b534:	8b 4b 0c             	mov    0xc(%ebx),%ecx
  44b537:	51                   	push   %ecx
  44b538:	8d 47 5c             	lea    0x5c(%edi),%eax
  44b53b:	e8 c0 ab fb ff       	call   0x406100 ; Timer::setCurrent
  44b540:	8b 97 a4 03 00 00    	mov    0x3a4(%edi),%edx
  44b546:	03 53 08             	add    0x8(%ebx),%edx
  44b549:	89 97 a8 03 00 00    	mov    %edx,0x3a8(%edi)
  44b54f:	eb b8                	jmp    0x44b509
  44b551:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44b555:	8d 73 08             	lea    0x8(%ebx),%esi
  44b558:	8b c6                	mov    %esi,%eax
  44b55a:	74 07                	je     0x44b563
  44b55c:	8b d7                	mov    %edi,%edx
  44b55e:	e8 9d fe ff ff       	call   0x44b400 ; getIntVar
  44b563:	ff 08                	decl   (%eax)
  44b565:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44b569:	8b 06                	mov    (%esi),%eax
  44b56b:	74 07                	je     0x44b574
  44b56d:	8b cf                	mov    %edi,%ecx
  44b56f:	e8 2c fd ff ff       	call   0x44b2a0 ; getNextInstruction
  44b574:	85 c0                	test   %eax,%eax
  44b576:	0f 8e 98 22 00 00    	jle    0x44d814
  44b57c:	8b 43 10             	mov    0x10(%ebx),%eax
  44b57f:	50                   	push   %eax
  44b580:	8d 47 5c             	lea    0x5c(%edi),%eax
  44b583:	e8 78 ab fb ff       	call   0x406100 ; Timer::setCurrent
  44b588:	8b 8f a4 03 00 00    	mov    0x3a4(%edi),%ecx
  44b58e:	03 4b 0c             	add    0xc(%ebx),%ecx
  44b591:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44b597:	e9 6d ff ff ff       	jmp    0x44b509
  44b59c:	83 a7 04 04 00 00 fe 	andl   $0xfffffffe,0x404(%edi)
  44b5a3:	0f b7 8f 7c 03 00 00 	movzwl 0x37c(%edi),%ecx
  44b5aa:	66 85 c9             	test   %cx,%cx
  44b5ad:	0f 84 df 0c 00 00    	je     0x44c292
; Interrupt:
  44b5b3:	8b b7 a4 03 00 00    	mov    0x3a4(%edi),%esi
  44b5b9:	33 d2                	xor    %edx,%edx
  44b5bb:	eb 03                	jmp    0x44b5c0
  44b5bd:	8d 49 00             	lea    0x0(%ecx),%ecx
  44b5c0:	0f b7 06             	movzwl (%esi),%eax
  44b5c3:	66 83 f8 40          	cmp    $0x40,%ax
  44b5c7:	75 08                	jne    0x44b5d1
  44b5c9:	0f bf d9             	movswl %cx,%ebx
  44b5cc:	3b 5e 08             	cmp    0x8(%esi),%ebx
  44b5cf:	74 1c                	je     0x44b5ed
  44b5d1:	66 83 f8 ff          	cmp    $0xffff,%ax
  44b5d5:	74 16                	je     0x44b5ed
  44b5d7:	66 83 f8 40          	cmp    $0x40,%ax
  44b5db:	75 08                	jne    0x44b5e5
  44b5dd:	83 7e 08 ff          	cmpl   $0xffffffff,0x8(%esi)
  44b5e1:	75 02                	jne    0x44b5e5
  44b5e3:	8b d6                	mov    %esi,%edx
  44b5e5:	0f b7 46 02          	movzwl 0x2(%esi),%eax
  44b5e9:	03 f0                	add    %eax,%esi
  44b5eb:	eb d3                	jmp    0x44b5c0
  44b5ed:	81 a7 04 04 00 00 ff 	andl   $0xffffefff,0x404(%edi)
  44b5f4:	ef ff ff 
  44b5f7:	33 c9                	xor    %ecx,%ecx
  44b5f9:	66 89 8f 7c 03 00 00 	mov    %cx,0x37c(%edi)
  44b600:	66 83 3e 40          	cmpw   $0x40,(%esi)
  44b604:	74 0a                	je     0x44b610
  44b606:	85 d2                	test   %edx,%edx
  44b608:	0f 84 8e 0c 00 00    	je     0x44c29c
  44b60e:	8b f2                	mov    %edx,%esi
  44b610:	8b 57 5c             	mov    0x5c(%edi),%edx
  44b613:	8b 4f 60             	mov    0x60(%edi),%ecx
  44b616:	8d 47 5c             	lea    0x5c(%edi),%eax
  44b619:	89 97 80 03 00 00    	mov    %edx,0x380(%edi)
  44b61f:	8b 50 08             	mov    0x8(%eax),%edx
  44b622:	89 8f 84 03 00 00    	mov    %ecx,0x384(%edi)
  44b628:	8b 48 0c             	mov    0xc(%eax),%ecx
  44b62b:	89 97 88 03 00 00    	mov    %edx,0x388(%edi)
  44b631:	8b 50 10             	mov    0x10(%eax),%edx
  44b634:	89 8f 8c 03 00 00    	mov    %ecx,0x38c(%edi)
  44b63a:	8b 8f a8 03 00 00    	mov    0x3a8(%edi),%ecx
  44b640:	89 97 90 03 00 00    	mov    %edx,0x390(%edi)
  44b646:	89 8f 94 03 00 00    	mov    %ecx,0x394(%edi)
  44b64c:	0f bf 56 04          	movswl 0x4(%esi),%edx
  44b650:	52                   	push   %edx
  44b651:	e8 aa aa fb ff       	call   0x406100 ; Timer::setCurrent
  44b656:	0f b7 46 02          	movzwl 0x2(%esi),%eax
  44b65a:	03 c6                	add    %esi,%eax
  44b65c:	83 8f 04 04 00 00 01 	orl    $0x1,0x404(%edi)
  44b663:	89 87 a8 03 00 00    	mov    %eax,0x3a8(%edi)
  44b669:	e9 9b fe ff ff       	jmp    0x44b509
  44b66e:	8b 8f 80 03 00 00    	mov    0x380(%edi),%ecx
  44b674:	8b 97 84 03 00 00    	mov    0x384(%edi),%edx
  44b67a:	8b 87 88 03 00 00    	mov    0x388(%edi),%eax
  44b680:	89 4f 5c             	mov    %ecx,0x5c(%edi)
  44b683:	8b 8f 8c 03 00 00    	mov    0x38c(%edi),%ecx
  44b689:	89 57 60             	mov    %edx,0x60(%edi)
  44b68c:	8b 97 90 03 00 00    	mov    0x390(%edi),%edx
  44b692:	89 47 64             	mov    %eax,0x64(%edi)
  44b695:	8b 87 94 03 00 00    	mov    0x394(%edi),%eax
  44b69b:	89 4f 68             	mov    %ecx,0x68(%edi)
  44b69e:	89 57 6c             	mov    %edx,0x6c(%edi)
  44b6a1:	89 87 a8 03 00 00    	mov    %eax,0x3a8(%edi)
  44b6a7:	e9 5d fe ff ff       	jmp    0x44b509
  44b6ac:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44b6b0:	74 0e                	je     0x44b6c0
  44b6b2:	8b 43 08             	mov    0x8(%ebx),%eax
  44b6b5:	8b cf                	mov    %edi,%ecx
  44b6b7:	e8 e4 fb ff ff       	call   0x44b2a0 ; getNextInstruction
  44b6bc:	8b f0                	mov    %eax,%esi
  44b6be:	eb 03                	jmp    0x44b6c3
  44b6c0:	8b 73 08             	mov    0x8(%ebx),%esi
  44b6c3:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44b6c7:	8b 43 0c             	mov    0xc(%ebx),%eax
  44b6ca:	74 07                	je     0x44b6d3
  44b6cc:	8b cf                	mov    %edi,%ecx
  44b6ce:	e8 cd fb ff ff       	call   0x44b2a0 ; getNextInstruction
  44b6d3:	3b f0                	cmp    %eax,%esi
  44b6d5:	0f 85 39 21 00 00    	jne    0x44d814
  44b6db:	e9 a9 02 00 00       	jmp    0x44b989
  44b6e0:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44b6e4:	d9 43 08             	flds   0x8(%ebx)
  44b6e7:	74 0b                	je     0x44b6f4
  44b6e9:	51                   	push   %ecx
  44b6ea:	8b c7                	mov    %edi,%eax
  44b6ec:	d9 1c 24             	fstps  (%esp)
  44b6ef:	e8 8c f9 ff ff       	call   0x44b080 ; getFloat
  44b6f4:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44b6f8:	d9 5c 24 18          	fstps  0x18(%esp)
  44b6fc:	d9 43 0c             	flds   0xc(%ebx)
  44b6ff:	74 0b                	je     0x44b70c
  44b701:	51                   	push   %ecx
  44b702:	8b c7                	mov    %edi,%eax
  44b704:	d9 1c 24             	fstps  (%esp)
  44b707:	e8 74 f9 ff ff       	call   0x44b080
  44b70c:	d9 5c 24 14          	fstps  0x14(%esp)
  44b710:	d9 44 24 18          	flds   0x18(%esp)
  44b714:	d9 44 24 14          	flds   0x14(%esp)
  44b718:	da e9                	fucompp
  44b71a:	df e0                	fnstsw %ax
  44b71c:	f6 c4 44             	test   $0x44,%ah
  44b71f:	e9 5f 02 00 00       	jmp    0x44b983
  44b724:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44b728:	74 0e                	je     0x44b738
  44b72a:	8b 43 08             	mov    0x8(%ebx),%eax
  44b72d:	8b cf                	mov    %edi,%ecx
  44b72f:	e8 6c fb ff ff       	call   0x44b2a0
  44b734:	8b f0                	mov    %eax,%esi
  44b736:	eb 03                	jmp    0x44b73b
  44b738:	8b 73 08             	mov    0x8(%ebx),%esi
  44b73b:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44b73f:	8b 43 0c             	mov    0xc(%ebx),%eax
  44b742:	74 07                	je     0x44b74b
  44b744:	8b cf                	mov    %edi,%ecx
  44b746:	e8 55 fb ff ff       	call   0x44b2a0
  44b74b:	3b f0                	cmp    %eax,%esi
  44b74d:	0f 84 c1 20 00 00    	je     0x44d814
  44b753:	e9 31 02 00 00       	jmp    0x44b989
  44b758:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44b75c:	d9 43 08             	flds   0x8(%ebx)
  44b75f:	74 0b                	je     0x44b76c
  44b761:	51                   	push   %ecx
  44b762:	8b c7                	mov    %edi,%eax
  44b764:	d9 1c 24             	fstps  (%esp)
  44b767:	e8 14 f9 ff ff       	call   0x44b080
  44b76c:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44b770:	d9 5c 24 14          	fstps  0x14(%esp)
  44b774:	d9 43 0c             	flds   0xc(%ebx)
  44b777:	74 0b                	je     0x44b784
  44b779:	51                   	push   %ecx
  44b77a:	8b c7                	mov    %edi,%eax
  44b77c:	d9 1c 24             	fstps  (%esp)
  44b77f:	e8 fc f8 ff ff       	call   0x44b080
  44b784:	d9 5c 24 18          	fstps  0x18(%esp)
  44b788:	d9 44 24 14          	flds   0x14(%esp)
  44b78c:	d9 44 24 18          	flds   0x18(%esp)
  44b790:	da e9                	fucompp
  44b792:	df e0                	fnstsw %ax
  44b794:	f6 c4 44             	test   $0x44,%ah
  44b797:	0f 8b 77 20 00 00    	jnp    0x44d814
  44b79d:	e9 e7 01 00 00       	jmp    0x44b989
  44b7a2:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44b7a6:	74 0e                	je     0x44b7b6
  44b7a8:	8b 43 08             	mov    0x8(%ebx),%eax
  44b7ab:	8b cf                	mov    %edi,%ecx
  44b7ad:	e8 ee fa ff ff       	call   0x44b2a0
  44b7b2:	8b f0                	mov    %eax,%esi
  44b7b4:	eb 03                	jmp    0x44b7b9
  44b7b6:	8b 73 08             	mov    0x8(%ebx),%esi
  44b7b9:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44b7bd:	8b 43 0c             	mov    0xc(%ebx),%eax
  44b7c0:	74 07                	je     0x44b7c9
  44b7c2:	8b cf                	mov    %edi,%ecx
  44b7c4:	e8 d7 fa ff ff       	call   0x44b2a0
  44b7c9:	3b f0                	cmp    %eax,%esi
  44b7cb:	0f 8d 43 20 00 00    	jge    0x44d814
  44b7d1:	e9 b3 01 00 00       	jmp    0x44b989
  44b7d6:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44b7da:	d9 43 08             	flds   0x8(%ebx)
  44b7dd:	74 0b                	je     0x44b7ea
  44b7df:	51                   	push   %ecx
  44b7e0:	8b c7                	mov    %edi,%eax
  44b7e2:	d9 1c 24             	fstps  (%esp)
  44b7e5:	e8 96 f8 ff ff       	call   0x44b080
  44b7ea:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44b7ee:	d9 5c 24 14          	fstps  0x14(%esp)
  44b7f2:	d9 43 0c             	flds   0xc(%ebx)
  44b7f5:	74 0b                	je     0x44b802
  44b7f7:	51                   	push   %ecx
  44b7f8:	8b c7                	mov    %edi,%eax
  44b7fa:	d9 1c 24             	fstps  (%esp)
  44b7fd:	e8 7e f8 ff ff       	call   0x44b080
  44b802:	d9 5c 24 18          	fstps  0x18(%esp)
  44b806:	d9 44 24 14          	flds   0x14(%esp)
  44b80a:	d9 44 24 18          	flds   0x18(%esp)
  44b80e:	de d9                	fcompp
  44b810:	df e0                	fnstsw %ax
  44b812:	f6 c4 41             	test   $0x41,%ah
  44b815:	0f 85 f9 1f 00 00    	jne    0x44d814
  44b81b:	e9 69 01 00 00       	jmp    0x44b989
  44b820:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44b824:	74 0e                	je     0x44b834
  44b826:	8b 43 08             	mov    0x8(%ebx),%eax
  44b829:	8b cf                	mov    %edi,%ecx
  44b82b:	e8 70 fa ff ff       	call   0x44b2a0
  44b830:	8b f0                	mov    %eax,%esi
  44b832:	eb 03                	jmp    0x44b837
  44b834:	8b 73 08             	mov    0x8(%ebx),%esi
  44b837:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44b83b:	8b 43 0c             	mov    0xc(%ebx),%eax
  44b83e:	74 07                	je     0x44b847
  44b840:	8b cf                	mov    %edi,%ecx
  44b842:	e8 59 fa ff ff       	call   0x44b2a0
  44b847:	3b f0                	cmp    %eax,%esi
  44b849:	0f 8f c5 1f 00 00    	jg     0x44d814
  44b84f:	e9 35 01 00 00       	jmp    0x44b989
  44b854:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44b858:	d9 43 08             	flds   0x8(%ebx)
  44b85b:	74 0b                	je     0x44b868
  44b85d:	51                   	push   %ecx
  44b85e:	8b c7                	mov    %edi,%eax
  44b860:	d9 1c 24             	fstps  (%esp)
  44b863:	e8 18 f8 ff ff       	call   0x44b080
  44b868:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44b86c:	d9 5c 24 14          	fstps  0x14(%esp)
  44b870:	d9 43 0c             	flds   0xc(%ebx)
  44b873:	74 0b                	je     0x44b880
  44b875:	51                   	push   %ecx
  44b876:	8b c7                	mov    %edi,%eax
  44b878:	d9 1c 24             	fstps  (%esp)
  44b87b:	e8 00 f8 ff ff       	call   0x44b080
  44b880:	d9 5c 24 18          	fstps  0x18(%esp)
  44b884:	d9 44 24 14          	flds   0x14(%esp)
  44b888:	d9 44 24 18          	flds   0x18(%esp)
  44b88c:	de d9                	fcompp
  44b88e:	df e0                	fnstsw %ax
  44b890:	f6 c4 01             	test   $0x1,%ah
  44b893:	0f 85 7b 1f 00 00    	jne    0x44d814
  44b899:	e9 eb 00 00 00       	jmp    0x44b989
  44b89e:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44b8a2:	74 0e                	je     0x44b8b2
  44b8a4:	8b 43 08             	mov    0x8(%ebx),%eax
  44b8a7:	8b cf                	mov    %edi,%ecx
  44b8a9:	e8 f2 f9 ff ff       	call   0x44b2a0
  44b8ae:	8b f0                	mov    %eax,%esi
  44b8b0:	eb 03                	jmp    0x44b8b5
  44b8b2:	8b 73 08             	mov    0x8(%ebx),%esi
  44b8b5:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44b8b9:	8b 43 0c             	mov    0xc(%ebx),%eax
  44b8bc:	74 07                	je     0x44b8c5
  44b8be:	8b cf                	mov    %edi,%ecx
  44b8c0:	e8 db f9 ff ff       	call   0x44b2a0
  44b8c5:	3b f0                	cmp    %eax,%esi
  44b8c7:	0f 8e 47 1f 00 00    	jle    0x44d814
  44b8cd:	e9 b7 00 00 00       	jmp    0x44b989
  44b8d2:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44b8d6:	d9 43 08             	flds   0x8(%ebx)
  44b8d9:	74 0b                	je     0x44b8e6
  44b8db:	51                   	push   %ecx
  44b8dc:	8b c7                	mov    %edi,%eax
  44b8de:	d9 1c 24             	fstps  (%esp)
  44b8e1:	e8 9a f7 ff ff       	call   0x44b080
  44b8e6:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44b8ea:	d9 5c 24 14          	fstps  0x14(%esp)
  44b8ee:	d9 43 0c             	flds   0xc(%ebx)
  44b8f1:	74 0b                	je     0x44b8fe
  44b8f3:	51                   	push   %ecx
  44b8f4:	8b c7                	mov    %edi,%eax
  44b8f6:	d9 1c 24             	fstps  (%esp)
  44b8f9:	e8 82 f7 ff ff       	call   0x44b080
  44b8fe:	d9 5c 24 18          	fstps  0x18(%esp)
  44b902:	d9 44 24 14          	flds   0x14(%esp)
  44b906:	d9 44 24 18          	flds   0x18(%esp)
  44b90a:	de d9                	fcompp
  44b90c:	df e0                	fnstsw %ax
  44b90e:	f6 c4 05             	test   $0x5,%ah
  44b911:	eb 70                	jmp    0x44b983
  44b913:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44b917:	74 0e                	je     0x44b927
  44b919:	8b 43 08             	mov    0x8(%ebx),%eax
  44b91c:	8b cf                	mov    %edi,%ecx
  44b91e:	e8 7d f9 ff ff       	call   0x44b2a0
  44b923:	8b f0                	mov    %eax,%esi
  44b925:	eb 03                	jmp    0x44b92a
  44b927:	8b 73 08             	mov    0x8(%ebx),%esi
  44b92a:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44b92e:	8b 43 0c             	mov    0xc(%ebx),%eax
  44b931:	74 07                	je     0x44b93a
  44b933:	8b cf                	mov    %edi,%ecx
  44b935:	e8 66 f9 ff ff       	call   0x44b2a0
  44b93a:	3b f0                	cmp    %eax,%esi
  44b93c:	0f 8c d2 1e 00 00    	jl     0x44d814
  44b942:	eb 45                	jmp    0x44b989
  44b944:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44b948:	d9 43 08             	flds   0x8(%ebx)
  44b94b:	74 0b                	je     0x44b958
  44b94d:	51                   	push   %ecx
  44b94e:	8b c7                	mov    %edi,%eax
  44b950:	d9 1c 24             	fstps  (%esp)
  44b953:	e8 28 f7 ff ff       	call   0x44b080
  44b958:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44b95c:	d9 5c 24 14          	fstps  0x14(%esp)
  44b960:	d9 43 0c             	flds   0xc(%ebx)
  44b963:	74 0b                	je     0x44b970
  44b965:	51                   	push   %ecx
  44b966:	8b c7                	mov    %edi,%eax
  44b968:	d9 1c 24             	fstps  (%esp)
  44b96b:	e8 10 f7 ff ff       	call   0x44b080
  44b970:	d9 5c 24 18          	fstps  0x18(%esp)
  44b974:	d9 44 24 14          	flds   0x14(%esp)
  44b978:	d9 44 24 18          	flds   0x18(%esp)
  44b97c:	de d9                	fcompp
  44b97e:	df e0                	fnstsw %ax
  44b980:	f6 c4 41             	test   $0x41,%ah
  44b983:	0f 8a 8b 1e 00 00    	jp     0x44d814
  44b989:	8b 4b 14             	mov    0x14(%ebx),%ecx
  44b98c:	51                   	push   %ecx
  44b98d:	8d 47 5c             	lea    0x5c(%edi),%eax
  44b990:	e8 6b a7 fb ff       	call   0x406100
  44b995:	8b 97 a4 03 00 00    	mov    0x3a4(%edi),%edx
  44b99b:	03 53 10             	add    0x10(%ebx),%edx
  44b99e:	89 97 a8 03 00 00    	mov    %edx,0x3a8(%edi)
  44b9a4:	e9 60 fb ff ff       	jmp    0x44b509
  44b9a9:	83 a7 04 04 00 00 fe 	andl   $0xfffffffe,0x404(%edi)
  44b9b0:	d9 44 24 74          	flds   0x74(%esp)
  44b9b4:	c7 87 a8 03 00 00 00 	movl   $0x0,0x3a8(%edi)
  44b9bb:	00 00 00 
  44b9be:	d9 1d 48 79 4a 00    	fstps  0x4a7948
  44b9c4:	b8 01 00 00 00       	mov    $0x1,%eax
  44b9c9:	5f                   	pop    %edi
  44b9ca:	5e                   	pop    %esi
  44b9cb:	5b                   	pop    %ebx
  44b9cc:	8b e5                	mov    %ebp,%esp
  44b9ce:	5d                   	pop    %ebp
  44b9cf:	c2 04 00             	ret    $0x4
  44b9d2:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44b9d6:	8b 43 08             	mov    0x8(%ebx),%eax
  44b9d9:	74 07                	je     0x44b9e2
  44b9db:	8b cf                	mov    %edi,%ecx
  44b9dd:	e8 be f8 ff ff       	call   0x44b2a0
  44b9e2:	c1 e0 1d             	shl    $0x1d,%eax
  44b9e5:	33 87 04 04 00 00    	xor    0x404(%edi),%eax
  44b9eb:	25 00 00 00 20       	and    $0x20000000,%eax
  44b9f0:	e9 19 1e 00 00       	jmp    0x44d80e
  44b9f5:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44b9f9:	8b 43 08             	mov    0x8(%ebx),%eax
  44b9fc:	74 07                	je     0x44ba05
  44b9fe:	8b cf                	mov    %edi,%ecx
  44ba00:	e8 9b f8 ff ff       	call   0x44b2a0
  44ba05:	8b 4f 20             	mov    0x20(%edi),%ecx
  44ba08:	8b 97 b0 03 00 00    	mov    0x3b0(%edi),%edx
  44ba0e:	51                   	push   %ecx
  44ba0f:	50                   	push   %eax
  44ba10:	52                   	push   %edx
  44ba11:	8d b4 24 a0 00 00 00 	lea    0xa0(%esp),%esi
  44ba18:	e8 e3 9f 00 00       	call   0x455a00
  44ba1d:	e9 a1 01 00 00       	jmp    0x44bbc3
  44ba22:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44ba26:	8b 43 08             	mov    0x8(%ebx),%eax
  44ba29:	74 07                	je     0x44ba32
  44ba2b:	8b cf                	mov    %edi,%ecx
  44ba2d:	e8 6e f8 ff ff       	call   0x44b2a0
  44ba32:	8b 57 20             	mov    0x20(%edi),%edx
  44ba35:	52                   	push   %edx
  44ba36:	50                   	push   %eax
  44ba37:	8b 87 b0 03 00 00    	mov    0x3b0(%edi),%eax
  44ba3d:	50                   	push   %eax
  44ba3e:	8d b4 24 9c 00 00 00 	lea    0x9c(%esp),%esi
  44ba45:	e8 b6 9f 00 00       	call   0x455a00
  44ba4a:	e8 d1 aa 00 00       	call   0x456520
  44ba4f:	8b f0                	mov    %eax,%esi
  44ba51:	8d 46 10             	lea    0x10(%esi),%eax
  44ba54:	8d 4f 10             	lea    0x10(%edi),%ecx
  44ba57:	e8 b4 2f fc ff       	call   0x40ea10
  44ba5c:	d9 43 0c             	flds   0xc(%ebx)
  44ba5f:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44ba63:	74 0b                	je     0x44ba70
  44ba65:	51                   	push   %ecx
  44ba66:	8b c7                	mov    %edi,%eax
  44ba68:	d9 1c 24             	fstps  (%esp)
  44ba6b:	e8 10 f6 ff ff       	call   0x44b080
  44ba70:	d9 5c 24 14          	fstps  0x14(%esp)
  44ba74:	d9 44 24 14          	flds   0x14(%esp)
  44ba78:	d9 9e f4 03 00 00    	fstps  0x3f4(%esi)
  44ba7e:	f6 43 06 04          	testb  $0x4,0x6(%ebx)
  44ba82:	d9 43 10             	flds   0x10(%ebx)
  44ba85:	74 0b                	je     0x44ba92
  44ba87:	51                   	push   %ecx
  44ba88:	8b c7                	mov    %edi,%eax
  44ba8a:	d9 1c 24             	fstps  (%esp)
  44ba8d:	e8 ee f5 ff ff       	call   0x44b080
  44ba92:	d9 5c 24 14          	fstps  0x14(%esp)
  44ba96:	d9 44 24 14          	flds   0x14(%esp)
  44ba9a:	d9 9e f8 03 00 00    	fstps  0x3f8(%esi)
  44baa0:	e9 db 01 00 00       	jmp    0x44bc80
  44baa5:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44baa9:	8b 43 08             	mov    0x8(%ebx),%eax
  44baac:	74 07                	je     0x44bab5
  44baae:	8b cf                	mov    %edi,%ecx
  44bab0:	e8 eb f7 ff ff       	call   0x44b2a0
  44bab5:	8b 57 20             	mov    0x20(%edi),%edx
  44bab8:	52                   	push   %edx
  44bab9:	50                   	push   %eax
  44baba:	8b 87 b0 03 00 00    	mov    0x3b0(%edi),%eax
  44bac0:	50                   	push   %eax
  44bac1:	8d b4 24 a4 00 00 00 	lea    0xa4(%esp),%esi
  44bac8:	e8 33 9f 00 00       	call   0x455a00
  44bacd:	e8 4e aa 00 00       	call   0x456520
  44bad2:	d9 43 0c             	flds   0xc(%ebx)
  44bad5:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44bad9:	8b f0                	mov    %eax,%esi
  44badb:	74 0b                	je     0x44bae8
  44badd:	51                   	push   %ecx
  44bade:	8b c7                	mov    %edi,%eax
  44bae0:	d9 1c 24             	fstps  (%esp)
  44bae3:	e8 98 f5 ff ff       	call   0x44b080
  44bae8:	d9 5c 24 14          	fstps  0x14(%esp)
  44baec:	d9 44 24 14          	flds   0x14(%esp)
  44baf0:	d9 9e f4 03 00 00    	fstps  0x3f4(%esi)
  44baf6:	f6 43 06 04          	testb  $0x4,0x6(%ebx)
  44bafa:	d9 43 10             	flds   0x10(%ebx)
  44bafd:	74 0b                	je     0x44bb0a
  44baff:	51                   	push   %ecx
  44bb00:	8b c7                	mov    %edi,%eax
  44bb02:	d9 1c 24             	fstps  (%esp)
  44bb05:	e8 76 f5 ff ff       	call   0x44b080
  44bb0a:	d9 5c 24 14          	fstps  0x14(%esp)
  44bb0e:	d9 44 24 14          	flds   0x14(%esp)
  44bb12:	d9 9e f8 03 00 00    	fstps  0x3f8(%esi)
  44bb18:	e9 63 01 00 00       	jmp    0x44bc80
  44bb1d:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44bb21:	8b 43 08             	mov    0x8(%ebx),%eax
  44bb24:	74 07                	je     0x44bb2d
  44bb26:	8b cf                	mov    %edi,%ecx
  44bb28:	e8 73 f7 ff ff       	call   0x44b2a0
  44bb2d:	8b 57 20             	mov    0x20(%edi),%edx
  44bb30:	52                   	push   %edx
  44bb31:	50                   	push   %eax
  44bb32:	8b 87 b0 03 00 00    	mov    0x3b0(%edi),%eax
  44bb38:	50                   	push   %eax
  44bb39:	8d b4 24 90 00 00 00 	lea    0x90(%esp),%esi
  44bb40:	e8 bb 9e 00 00       	call   0x455a00
  44bb45:	e8 d6 a9 00 00       	call   0x456520
  44bb4a:	8b 8f dc 03 00 00    	mov    0x3dc(%edi),%ecx
  44bb50:	89 88 f4 03 00 00    	mov    %ecx,0x3f4(%eax)
  44bb56:	8b 97 e0 03 00 00    	mov    0x3e0(%edi),%edx
  44bb5c:	89 90 f8 03 00 00    	mov    %edx,0x3f8(%eax)
  44bb62:	8b 8f e4 03 00 00    	mov    0x3e4(%edi),%ecx
  44bb68:	89 88 fc 03 00 00    	mov    %ecx,0x3fc(%eax)
  44bb6e:	8b 97 e8 03 00 00    	mov    0x3e8(%edi),%edx
  44bb74:	05 e8 03 00 00       	add    $0x3e8,%eax
  44bb79:	89 10                	mov    %edx,(%eax)
  44bb7b:	8b 8f ec 03 00 00    	mov    0x3ec(%edi),%ecx
  44bb81:	89 48 04             	mov    %ecx,0x4(%eax)
  44bb84:	8b 97 f0 03 00 00    	mov    0x3f0(%edi),%edx
  44bb8a:	89 50 08             	mov    %edx,0x8(%eax)
  44bb8d:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44bb91:	03 cb                	add    %ebx,%ecx
  44bb93:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44bb99:	e9 6b f9 ff ff       	jmp    0x44b509
  44bb9e:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44bba2:	8b 43 08             	mov    0x8(%ebx),%eax
  44bba5:	74 07                	je     0x44bbae
  44bba7:	8b cf                	mov    %edi,%ecx
  44bba9:	e8 f2 f6 ff ff       	call   0x44b2a0
  44bbae:	8b 4f 20             	mov    0x20(%edi),%ecx
  44bbb1:	8b 97 b0 03 00 00    	mov    0x3b0(%edi),%edx
  44bbb7:	51                   	push   %ecx
  44bbb8:	50                   	push   %eax
  44bbb9:	52                   	push   %edx
  44bbba:	8d 74 24 48          	lea    0x48(%esp),%esi
  44bbbe:	e8 fd 9f 00 00       	call   0x455bc0
  44bbc3:	e8 58 a9 00 00       	call   0x456520
  44bbc8:	8b f0                	mov    %eax,%esi
  44bbca:	8d 46 10             	lea    0x10(%esi),%eax
  44bbcd:	8d 4f 10             	lea    0x10(%edi),%ecx
  44bbd0:	e8 3b 2e fc ff       	call   0x40ea10
  44bbd5:	8b 87 dc 03 00 00    	mov    0x3dc(%edi),%eax
  44bbdb:	89 86 f4 03 00 00    	mov    %eax,0x3f4(%esi)
  44bbe1:	8b 8f e0 03 00 00    	mov    0x3e0(%edi),%ecx
  44bbe7:	89 8e f8 03 00 00    	mov    %ecx,0x3f8(%esi)
  44bbed:	8b 97 e4 03 00 00    	mov    0x3e4(%edi),%edx
  44bbf3:	89 96 fc 03 00 00    	mov    %edx,0x3fc(%esi)
  44bbf9:	e9 82 00 00 00       	jmp    0x44bc80
  44bbfe:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44bc02:	8b 43 08             	mov    0x8(%ebx),%eax
  44bc05:	74 07                	je     0x44bc0e
  44bc07:	8b cf                	mov    %edi,%ecx
  44bc09:	e8 92 f6 ff ff       	call   0x44b2a0
  44bc0e:	8b 57 20             	mov    0x20(%edi),%edx
  44bc11:	52                   	push   %edx
  44bc12:	50                   	push   %eax
  44bc13:	8b 87 b0 03 00 00    	mov    0x3b0(%edi),%eax
  44bc19:	50                   	push   %eax
  44bc1a:	8d 74 24 58          	lea    0x58(%esp),%esi
  44bc1e:	e8 cd a1 00 00       	call   0x455df0
  44bc23:	eb 25                	jmp    0x44bc4a
  44bc25:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44bc29:	8b 43 08             	mov    0x8(%ebx),%eax
  44bc2c:	74 07                	je     0x44bc35
  44bc2e:	8b cf                	mov    %edi,%ecx
  44bc30:	e8 6b f6 ff ff       	call   0x44b2a0
  44bc35:	8b 57 20             	mov    0x20(%edi),%edx
  44bc38:	52                   	push   %edx
  44bc39:	50                   	push   %eax
  44bc3a:	8b 87 b0 03 00 00    	mov    0x3b0(%edi),%eax
  44bc40:	50                   	push   %eax
  44bc41:	8d 74 24 28          	lea    0x28(%esp),%esi
  44bc45:	e8 d6 a3 00 00       	call   0x456020
  44bc4a:	e8 d1 a8 00 00       	call   0x456520
  44bc4f:	8b f0                	mov    %eax,%esi
  44bc51:	8d 46 10             	lea    0x10(%esi),%eax
  44bc54:	8d 4f 10             	lea    0x10(%edi),%ecx
  44bc57:	e8 b4 2d fc ff       	call   0x40ea10
  44bc5c:	8b 8f dc 03 00 00    	mov    0x3dc(%edi),%ecx
  44bc62:	89 8e f4 03 00 00    	mov    %ecx,0x3f4(%esi)
  44bc68:	8b 97 e0 03 00 00    	mov    0x3e0(%edi),%edx
  44bc6e:	89 96 f8 03 00 00    	mov    %edx,0x3f8(%esi)
  44bc74:	8b 87 e4 03 00 00    	mov    0x3e4(%edi),%eax
  44bc7a:	89 86 fc 03 00 00    	mov    %eax,0x3fc(%esi)
  44bc80:	8b 8f e8 03 00 00    	mov    0x3e8(%edi),%ecx
  44bc86:	89 8e e8 03 00 00    	mov    %ecx,0x3e8(%esi)
  44bc8c:	8b 97 ec 03 00 00    	mov    0x3ec(%edi),%edx
  44bc92:	8d 86 e8 03 00 00    	lea    0x3e8(%esi),%eax
  44bc98:	89 50 04             	mov    %edx,0x4(%eax)
  44bc9b:	8b 8f f0 03 00 00    	mov    0x3f0(%edi),%ecx
  44bca1:	89 48 08             	mov    %ecx,0x8(%eax)
  44bca4:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44bca8:	03 cb                	add    %ebx,%ecx
  44bcaa:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44bcb0:	e9 54 f8 ff ff       	jmp    0x44b509
  44bcb5:	b8 01 00 00 00       	mov    $0x1,%eax
  44bcba:	09 87 04 04 00 00    	or     %eax,0x404(%edi)
  44bcc0:	83 bf 2c 04 00 00 00 	cmpl   $0x0,0x42c(%edi)
  44bcc7:	74 1d                	je     0x44bce6
  44bcc9:	84 43 06             	test   %al,0x6(%ebx)
  44bccc:	8b 43 08             	mov    0x8(%ebx),%eax
  44bccf:	74 07                	je     0x44bcd8
  44bcd1:	8b cf                	mov    %edi,%ecx
  44bcd3:	e8 c8 f5 ff ff       	call   0x44b2a0
  44bcd8:	8b d0                	mov    %eax,%edx
  44bcda:	8b 87 2c 04 00 00    	mov    0x42c(%edi),%eax
  44bce0:	8b cf                	mov    %edi,%ecx
  44bce2:	ff d0                	call   *%eax
  44bce4:	eb 0f                	jmp    0x44bcf5
  44bce6:	84 43 06             	test   %al,0x6(%ebx)
  44bce9:	8b 43 08             	mov    0x8(%ebx),%eax
  44bcec:	74 07                	je     0x44bcf5
  44bcee:	8b cf                	mov    %edi,%ecx
  44bcf0:	e8 ab f5 ff ff       	call   0x44b2a0
  44bcf5:	8b 97 b0 03 00 00    	mov    0x3b0(%edi),%edx
  44bcfb:	8b c8                	mov    %eax,%ecx
  44bcfd:	8b c7                	mov    %edi,%eax
  44bcff:	e8 3c ee ff ff       	call   0x44ab40
  44bd04:	8b 4f 60             	mov    0x60(%edi),%ecx
  44bd07:	89 8f 98 03 00 00    	mov    %ecx,0x398(%edi)
  44bd0d:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44bd11:	03 cb                	add    %ebx,%ecx
  44bd13:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44bd19:	e9 eb f7 ff ff       	jmp    0x44b509
  44bd1e:	b8 01 00 00 00       	mov    $0x1,%eax
  44bd23:	09 87 04 04 00 00    	or     %eax,0x404(%edi)
  44bd29:	83 bf 2c 04 00 00 00 	cmpl   $0x0,0x42c(%edi)
  44bd30:	74 50                	je     0x44bd82
  44bd32:	84 43 06             	test   %al,0x6(%ebx)
  44bd35:	74 10                	je     0x44bd47
  44bd37:	8b 43 08             	mov    0x8(%ebx),%eax
  44bd3a:	8b cf                	mov    %edi,%ecx
  44bd3c:	e8 5f f5 ff ff       	call   0x44b2a0
  44bd41:	89 44 24 18          	mov    %eax,0x18(%esp)
  44bd45:	eb 07                	jmp    0x44bd4e
  44bd47:	8b 53 08             	mov    0x8(%ebx),%edx
  44bd4a:	89 54 24 18          	mov    %edx,0x18(%esp)
  44bd4e:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44bd52:	8b 43 0c             	mov    0xc(%ebx),%eax
  44bd55:	74 07                	je     0x44bd5e
  44bd57:	8b cf                	mov    %edi,%ecx
  44bd59:	e8 42 f5 ff ff       	call   0x44b2a0
  44bd5e:	be 00 2f 4c 00       	mov    $0x4c2f00,%esi
  44bd63:	89 44 24 14          	mov    %eax,0x14(%esp)
  44bd67:	e8 c4 ce 00 00       	call   0x458c30
  44bd6c:	33 d2                	xor    %edx,%edx
  44bd6e:	f7 74 24 14          	divl   0x14(%esp)
  44bd72:	8b 87 2c 04 00 00    	mov    0x42c(%edi),%eax
  44bd78:	8b cf                	mov    %edi,%ecx
  44bd7a:	03 54 24 18          	add    0x18(%esp),%edx
  44bd7e:	ff d0                	call   *%eax
  44bd80:	eb 4f                	jmp    0x44bdd1
  44bd82:	84 43 06             	test   %al,0x6(%ebx)
  44bd85:	74 10                	je     0x44bd97
  44bd87:	8b 43 08             	mov    0x8(%ebx),%eax
  44bd8a:	8b cf                	mov    %edi,%ecx
  44bd8c:	e8 0f f5 ff ff       	call   0x44b2a0
  44bd91:	89 44 24 18          	mov    %eax,0x18(%esp)
  44bd95:	eb 07                	jmp    0x44bd9e
  44bd97:	8b 4b 08             	mov    0x8(%ebx),%ecx
  44bd9a:	89 4c 24 18          	mov    %ecx,0x18(%esp)
  44bd9e:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44bda2:	74 10                	je     0x44bdb4
  44bda4:	8b 43 0c             	mov    0xc(%ebx),%eax
  44bda7:	8b cf                	mov    %edi,%ecx
  44bda9:	e8 f2 f4 ff ff       	call   0x44b2a0
  44bdae:	89 44 24 14          	mov    %eax,0x14(%esp)
  44bdb2:	eb 07                	jmp    0x44bdbb
  44bdb4:	8b 53 0c             	mov    0xc(%ebx),%edx
  44bdb7:	89 54 24 14          	mov    %edx,0x14(%esp)
  44bdbb:	be 00 2f 4c 00       	mov    $0x4c2f00,%esi
  44bdc0:	e8 6b ce 00 00       	call   0x458c30
  44bdc5:	33 d2                	xor    %edx,%edx
  44bdc7:	f7 74 24 14          	divl   0x14(%esp)
  44bdcb:	8b c2                	mov    %edx,%eax
  44bdcd:	03 44 24 18          	add    0x18(%esp),%eax
  44bdd1:	8b 97 b0 03 00 00    	mov    0x3b0(%edi),%edx
  44bdd7:	8b c8                	mov    %eax,%ecx
  44bdd9:	8b c7                	mov    %edi,%eax
  44bddb:	e8 60 ed ff ff       	call   0x44ab40
  44bde0:	8b 47 60             	mov    0x60(%edi),%eax
  44bde3:	89 87 98 03 00 00    	mov    %eax,0x398(%edi)
  44bde9:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44bded:	03 cb                	add    %ebx,%ecx
  44bdef:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44bdf5:	e9 0f f7 ff ff       	jmp    0x44b509
  44bdfa:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44bdfe:	d9 43 08             	flds   0x8(%ebx)
  44be01:	74 0b                	je     0x44be0e
  44be03:	51                   	push   %ecx
  44be04:	8b c7                	mov    %edi,%eax
  44be06:	d9 1c 24             	fstps  (%esp)
  44be09:	e8 72 f2 ff ff       	call   0x44b080
  44be0e:	d9 5c 24 14          	fstps  0x14(%esp)
  44be12:	d9 44 24 14          	flds   0x14(%esp)
  44be16:	d9 5f 3c             	fstps  0x3c(%edi)
  44be19:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44be1d:	d9 43 0c             	flds   0xc(%ebx)
  44be20:	74 0b                	je     0x44be2d
  44be22:	51                   	push   %ecx
  44be23:	8b c7                	mov    %edi,%eax
  44be25:	d9 1c 24             	fstps  (%esp)
  44be28:	e8 53 f2 ff ff       	call   0x44b080
  44be2d:	83 8f 04 04 00 00 08 	orl    $0x8,0x404(%edi)
  44be34:	d9 5c 24 14          	fstps  0x14(%esp)
  44be38:	d9 44 24 14          	flds   0x14(%esp)
  44be3c:	d9 5f 40             	fstps  0x40(%edi)
  44be3f:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44be43:	03 cb                	add    %ebx,%ecx
  44be45:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44be4b:	e9 b9 f6 ff ff       	jmp    0x44b509
  44be50:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44be54:	8b 43 08             	mov    0x8(%ebx),%eax
  44be57:	74 07                	je     0x44be60
  44be59:	8b cf                	mov    %edi,%ecx
  44be5b:	e8 40 f4 ff ff       	call   0x44b2a0
  44be60:	88 87 77 03 00 00    	mov    %al,0x377(%edi)
  44be66:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44be6a:	03 cb                	add    %ebx,%ecx
  44be6c:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44be72:	e9 92 f6 ff ff       	jmp    0x44b509
  44be77:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44be7b:	8b 43 08             	mov    0x8(%ebx),%eax
  44be7e:	74 07                	je     0x44be87
  44be80:	8b cf                	mov    %edi,%ecx
  44be82:	e8 19 f4 ff ff       	call   0x44b2a0
  44be87:	88 87 76 03 00 00    	mov    %al,0x376(%edi)
  44be8d:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44be91:	8b 43 0c             	mov    0xc(%ebx),%eax
  44be94:	74 07                	je     0x44be9d
  44be96:	8b cf                	mov    %edi,%ecx
  44be98:	e8 03 f4 ff ff       	call   0x44b2a0
  44be9d:	88 87 75 03 00 00    	mov    %al,0x375(%edi)
  44bea3:	f6 43 06 04          	testb  $0x4,0x6(%ebx)
  44bea7:	8b 43 10             	mov    0x10(%ebx),%eax
  44beaa:	74 07                	je     0x44beb3
  44beac:	8b cf                	mov    %edi,%ecx
  44beae:	e8 ed f3 ff ff       	call   0x44b2a0
  44beb3:	88 87 74 03 00 00    	mov    %al,0x374(%edi)
  44beb9:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44bebd:	03 cb                	add    %ebx,%ecx
  44bebf:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44bec5:	e9 3f f6 ff ff       	jmp    0x44b509
  44beca:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44bece:	8b 43 08             	mov    0x8(%ebx),%eax
  44bed1:	74 07                	je     0x44beda
  44bed3:	8b cf                	mov    %edi,%ecx
  44bed5:	e8 c6 f3 ff ff       	call   0x44b2a0
  44beda:	88 87 7b 03 00 00    	mov    %al,0x37b(%edi)
  44bee0:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44bee4:	03 cb                	add    %ebx,%ecx
  44bee6:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44beec:	e9 18 f6 ff ff       	jmp    0x44b509
  44bef1:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44bef5:	8b 43 08             	mov    0x8(%ebx),%eax
  44bef8:	74 07                	je     0x44bf01
  44befa:	8b cf                	mov    %edi,%ecx
  44befc:	e8 9f f3 ff ff       	call   0x44b2a0
  44bf01:	88 87 7a 03 00 00    	mov    %al,0x37a(%edi)
  44bf07:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44bf0b:	8b 43 0c             	mov    0xc(%ebx),%eax
  44bf0e:	74 07                	je     0x44bf17
  44bf10:	8b cf                	mov    %edi,%ecx
  44bf12:	e8 89 f3 ff ff       	call   0x44b2a0
  44bf17:	88 87 79 03 00 00    	mov    %al,0x379(%edi)
  44bf1d:	f6 43 06 04          	testb  $0x4,0x6(%ebx)
  44bf21:	8b 43 10             	mov    0x10(%ebx),%eax
  44bf24:	74 07                	je     0x44bf2d
  44bf26:	8b cf                	mov    %edi,%ecx
  44bf28:	e8 73 f3 ff ff       	call   0x44b2a0
  44bf2d:	88 87 78 03 00 00    	mov    %al,0x378(%edi)
  44bf33:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44bf37:	03 cb                	add    %ebx,%ecx
  44bf39:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44bf3f:	e9 c5 f5 ff ff       	jmp    0x44b509
  44bf44:	8b 87 04 04 00 00    	mov    0x404(%edi),%eax
  44bf4a:	d9 47 3c             	flds   0x3c(%edi)
  44bf4d:	dc 0d a0 82 49 00    	fmull  0x4982a0
  44bf53:	35 00 02 00 00       	xor    $0x200,%eax
  44bf58:	83 c8 08             	or     $0x8,%eax
  44bf5b:	89 87 04 04 00 00    	mov    %eax,0x404(%edi)
  44bf61:	d9 5f 3c             	fstps  0x3c(%edi)
  44bf64:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44bf68:	03 cb                	add    %ebx,%ecx
  44bf6a:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44bf70:	e9 94 f5 ff ff       	jmp    0x44b509
  44bf75:	8b 87 04 04 00 00    	mov    0x404(%edi),%eax
  44bf7b:	d9 47 40             	flds   0x40(%edi)
  44bf7e:	dc 0d a0 82 49 00    	fmull  0x4982a0
  44bf84:	35 00 04 00 00       	xor    $0x400,%eax
  44bf89:	83 c8 08             	or     $0x8,%eax
  44bf8c:	89 87 04 04 00 00    	mov    %eax,0x404(%edi)
  44bf92:	d9 5f 40             	fstps  0x40(%edi)
  44bf95:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44bf99:	03 cb                	add    %ebx,%ecx
  44bf9b:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44bfa1:	e9 63 f5 ff ff       	jmp    0x44b509
  44bfa6:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44bfaa:	d9 43 08             	flds   0x8(%ebx)
  44bfad:	74 0b                	je     0x44bfba
  44bfaf:	51                   	push   %ecx
  44bfb0:	8b c7                	mov    %edi,%eax
  44bfb2:	d9 1c 24             	fstps  (%esp)
  44bfb5:	e8 c6 f0 ff ff       	call   0x44b080
  44bfba:	d9 5c 24 14          	fstps  0x14(%esp)
  44bfbe:	d9 44 24 14          	flds   0x14(%esp)
  44bfc2:	d9 5f 24             	fstps  0x24(%edi)
  44bfc5:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44bfc9:	d9 43 0c             	flds   0xc(%ebx)
  44bfcc:	74 0b                	je     0x44bfd9
  44bfce:	51                   	push   %ecx
  44bfcf:	8b c7                	mov    %edi,%eax
  44bfd1:	d9 1c 24             	fstps  (%esp)
  44bfd4:	e8 a7 f0 ff ff       	call   0x44b080
  44bfd9:	d9 5c 24 14          	fstps  0x14(%esp)
  44bfdd:	d9 44 24 14          	flds   0x14(%esp)
  44bfe1:	d9 5f 28             	fstps  0x28(%edi)
  44bfe4:	f6 43 06 04          	testb  $0x4,0x6(%ebx)
  44bfe8:	d9 43 10             	flds   0x10(%ebx)
  44bfeb:	74 0b                	je     0x44bff8
  44bfed:	51                   	push   %ecx
  44bfee:	8b c7                	mov    %edi,%eax
  44bff0:	d9 1c 24             	fstps  (%esp)
  44bff3:	e8 88 f0 ff ff       	call   0x44b080
  44bff8:	83 8f 04 04 00 00 04 	orl    $0x4,0x404(%edi)
  44bfff:	d9 5c 24 14          	fstps  0x14(%esp)
  44c003:	d9 44 24 14          	flds   0x14(%esp)
  44c007:	d9 5f 2c             	fstps  0x2c(%edi)
  44c00a:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44c00e:	03 cb                	add    %ebx,%ecx
  44c010:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44c016:	e9 ee f4 ff ff       	jmp    0x44b509
  44c01b:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44c01f:	d9 43 08             	flds   0x8(%ebx)
  44c022:	74 0b                	je     0x44c02f
  44c024:	51                   	push   %ecx
  44c025:	8b c7                	mov    %edi,%eax
  44c027:	d9 1c 24             	fstps  (%esp)
  44c02a:	e8 51 f0 ff ff       	call   0x44b080
  44c02f:	d9 5c 24 14          	fstps  0x14(%esp)
  44c033:	d9 44 24 14          	flds   0x14(%esp)
  44c037:	d9 5f 30             	fstps  0x30(%edi)
  44c03a:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44c03e:	d9 43 0c             	flds   0xc(%ebx)
  44c041:	74 0b                	je     0x44c04e
  44c043:	51                   	push   %ecx
  44c044:	8b c7                	mov    %edi,%eax
  44c046:	d9 1c 24             	fstps  (%esp)
  44c049:	e8 32 f0 ff ff       	call   0x44b080
  44c04e:	d9 5c 24 14          	fstps  0x14(%esp)
  44c052:	d9 44 24 14          	flds   0x14(%esp)
  44c056:	d9 5f 34             	fstps  0x34(%edi)
  44c059:	f6 43 06 04          	testb  $0x4,0x6(%ebx)
  44c05d:	d9 43 10             	flds   0x10(%ebx)
  44c060:	74 0b                	je     0x44c06d
  44c062:	51                   	push   %ecx
  44c063:	8b c7                	mov    %edi,%eax
  44c065:	d9 1c 24             	fstps  (%esp)
  44c068:	e8 13 f0 ff ff       	call   0x44b080
  44c06d:	83 8f 04 04 00 00 04 	orl    $0x4,0x404(%edi)
  44c074:	d9 5c 24 14          	fstps  0x14(%esp)
  44c078:	d9 44 24 14          	flds   0x14(%esp)
  44c07c:	d9 5f 38             	fstps  0x38(%edi)
  44c07f:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44c083:	03 cb                	add    %ebx,%ecx
  44c085:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44c08b:	e9 79 f4 ff ff       	jmp    0x44b509
  44c090:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44c094:	d9 43 08             	flds   0x8(%ebx)
  44c097:	74 0b                	je     0x44c0a4
  44c099:	51                   	push   %ecx
  44c09a:	8b c7                	mov    %edi,%eax
  44c09c:	d9 1c 24             	fstps  (%esp)
  44c09f:	e8 dc ef ff ff       	call   0x44b080
  44c0a4:	d9 5c 24 14          	fstps  0x14(%esp)
  44c0a8:	d9 44 24 14          	flds   0x14(%esp)
  44c0ac:	d9 5f 44             	fstps  0x44(%edi)
  44c0af:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44c0b3:	d9 43 0c             	flds   0xc(%ebx)
  44c0b6:	74 0b                	je     0x44c0c3
  44c0b8:	51                   	push   %ecx
  44c0b9:	8b c7                	mov    %edi,%eax
  44c0bb:	d9 1c 24             	fstps  (%esp)
  44c0be:	e8 bd ef ff ff       	call   0x44b080
  44c0c3:	d9 5c 24 14          	fstps  0x14(%esp)
  44c0c7:	d9 44 24 14          	flds   0x14(%esp)
  44c0cb:	d9 5f 48             	fstps  0x48(%edi)
  44c0ce:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44c0d2:	03 cb                	add    %ebx,%ecx
  44c0d4:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44c0da:	e9 2a f4 ff ff       	jmp    0x44b509
  44c0df:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44c0e3:	8b 43 0c             	mov    0xc(%ebx),%eax
  44c0e6:	74 07                	je     0x44c0ef
  44c0e8:	8b cf                	mov    %edi,%ecx
  44c0ea:	e8 b1 f1 ff ff       	call   0x44b2a0
  44c0ef:	0f b6 4b 08          	movzbl 0x8(%ebx),%ecx
  44c0f3:	0f b6 97 77 03 00 00 	movzbl 0x377(%edi),%edx
  44c0fa:	51                   	push   %ecx
  44c0fb:	52                   	push   %edx
  44c0fc:	8b c8                	mov    %eax,%ecx
  44c0fe:	6a 00                	push   $0x0
  44c100:	8b c7                	mov    %edi,%eax
  44c102:	e8 f9 32 00 00       	call   0x44f400
  44c107:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44c10b:	03 cb                	add    %ebx,%ecx
  44c10d:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44c113:	e9 f1 f3 ff ff       	jmp    0x44b509
  44c118:	8b 43 08             	mov    0x8(%ebx),%eax
  44c11b:	c1 e0 04             	shl    $0x4,%eax
  44c11e:	33 87 04 04 00 00    	xor    0x404(%edi),%eax
  44c124:	83 e0 70             	and    $0x70,%eax
  44c127:	e9 e2 16 00 00       	jmp    0x44d80e
  44c12c:	f7 87 04 04 00 00 00 	testl  $0x100,0x404(%edi)
  44c133:	01 00 00 
  44c136:	d9 43 10             	flds   0x10(%ebx)
  44c139:	0f 85 9e 00 00 00    	jne    0x44c1dd
  44c13f:	f6 43 06 04          	testb  $0x4,0x6(%ebx)
  44c143:	74 0b                	je     0x44c150
  44c145:	51                   	push   %ecx
  44c146:	8b c7                	mov    %edi,%eax
  44c148:	d9 1c 24             	fstps  (%esp)
  44c14b:	e8 30 ef ff ff       	call   0x44b080
  44c150:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44c154:	d9 5c 24 10          	fstps  0x10(%esp)
  44c158:	d9 43 0c             	flds   0xc(%ebx)
  44c15b:	74 0b                	je     0x44c168
  44c15d:	51                   	push   %ecx
  44c15e:	8b c7                	mov    %edi,%eax
  44c160:	d9 1c 24             	fstps  (%esp)
  44c163:	e8 18 ef ff ff       	call   0x44b080
  44c168:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44c16c:	d9 5c 24 18          	fstps  0x18(%esp)
  44c170:	d9 43 08             	flds   0x8(%ebx)
  44c173:	74 0b                	je     0x44c180
  44c175:	51                   	push   %ecx
  44c176:	8b c7                	mov    %edi,%eax
  44c178:	d9 1c 24             	fstps  (%esp)
  44c17b:	e8 00 ef ff ff       	call   0x44b080
  44c180:	d9 5c 24 14          	fstps  0x14(%esp)
  44c184:	d9 44 24 14          	flds   0x14(%esp)
  44c188:	d9 9c 24 9c 00 00 00 	fstps  0x9c(%esp)
  44c18f:	8b 8c 24 9c 00 00 00 	mov    0x9c(%esp),%ecx
  44c196:	d9 44 24 18          	flds   0x18(%esp)
  44c19a:	89 8f dc 03 00 00    	mov    %ecx,0x3dc(%edi)
  44c1a0:	d9 9c 24 a0 00 00 00 	fstps  0xa0(%esp)
  44c1a7:	8b 94 24 a0 00 00 00 	mov    0xa0(%esp),%edx
  44c1ae:	d9 44 24 10          	flds   0x10(%esp)
  44c1b2:	89 97 e0 03 00 00    	mov    %edx,0x3e0(%edi)
  44c1b8:	d9 9c 24 a4 00 00 00 	fstps  0xa4(%esp)
  44c1bf:	8b 84 24 a4 00 00 00 	mov    0xa4(%esp),%eax
  44c1c6:	89 87 e4 03 00 00    	mov    %eax,0x3e4(%edi)
  44c1cc:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44c1d0:	03 cb                	add    %ebx,%ecx
  44c1d2:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44c1d8:	e9 2c f3 ff ff       	jmp    0x44b509
  44c1dd:	f6 43 06 04          	testb  $0x4,0x6(%ebx)
  44c1e1:	74 0b                	je     0x44c1ee
  44c1e3:	51                   	push   %ecx
  44c1e4:	8b c7                	mov    %edi,%eax
  44c1e6:	d9 1c 24             	fstps  (%esp)
  44c1e9:	e8 92 ee ff ff       	call   0x44b080
  44c1ee:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44c1f2:	d9 5c 24 18          	fstps  0x18(%esp)
  44c1f6:	d9 43 0c             	flds   0xc(%ebx)
  44c1f9:	74 0b                	je     0x44c206
  44c1fb:	51                   	push   %ecx
  44c1fc:	8b c7                	mov    %edi,%eax
  44c1fe:	d9 1c 24             	fstps  (%esp)
  44c201:	e8 7a ee ff ff       	call   0x44b080
  44c206:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44c20a:	d9 5c 24 14          	fstps  0x14(%esp)
  44c20e:	d9 43 08             	flds   0x8(%ebx)
  44c211:	74 0b                	je     0x44c21e
  44c213:	51                   	push   %ecx
  44c214:	8b c7                	mov    %edi,%eax
  44c216:	d9 1c 24             	fstps  (%esp)
  44c219:	e8 62 ee ff ff       	call   0x44b080
  44c21e:	d9 5c 24 10          	fstps  0x10(%esp)
  44c222:	d9 44 24 10          	flds   0x10(%esp)
  44c226:	d9 5c 24 40          	fstps  0x40(%esp)
  44c22a:	8b 4c 24 40          	mov    0x40(%esp),%ecx
  44c22e:	d9 44 24 14          	flds   0x14(%esp)
  44c232:	89 8f f4 03 00 00    	mov    %ecx,0x3f4(%edi)
  44c238:	d9 5c 24 44          	fstps  0x44(%esp)
  44c23c:	8b 54 24 44          	mov    0x44(%esp),%edx
  44c240:	d9 44 24 18          	flds   0x18(%esp)
  44c244:	89 97 f8 03 00 00    	mov    %edx,0x3f8(%edi)
  44c24a:	d9 5c 24 48          	fstps  0x48(%esp)
  44c24e:	8b 44 24 48          	mov    0x48(%esp),%eax
  44c252:	89 87 fc 03 00 00    	mov    %eax,0x3fc(%edi)
  44c258:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44c25c:	03 cb                	add    %ebx,%ecx
  44c25e:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44c264:	e9 a0 f2 ff ff       	jmp    0x44b509
  44c269:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44c26d:	8b 43 08             	mov    0x8(%ebx),%eax
  44c270:	74 07                	je     0x44c279
  44c272:	8b cf                	mov    %edi,%ecx
  44c274:	e8 27 f0 ff ff       	call   0x44b2a0
  44c279:	8d 77 5c             	lea    0x5c(%edi),%esi
  44c27c:	e8 3f 93 fe ff       	call   0x4355c0
  44c281:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44c285:	03 cb                	add    %ebx,%ecx
  44c287:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44c28d:	e9 77 f2 ff ff       	jmp    0x44b509
  44c292:	81 8f 04 04 00 00 00 	orl    $0x1000,0x404(%edi)
  44c299:	10 00 00 
  44c29c:	d9 05 20 7f 49 00    	flds   0x497f20
  44c2a2:	51                   	push   %ecx
  44c2a3:	8d 77 5c             	lea    0x5c(%edi),%esi
  44c2a6:	d9 1c 24             	fstps  (%esp)
  44c2a9:	e8 62 cf 00 00       	call   0x459210
  44c2ae:	d9 ee                	fldz
  44c2b0:	d8 5f 30             	fcomps 0x30(%edi)
  44c2b3:	df e0                	fnstsw %ax
  44c2b5:	f6 c4 44             	test   $0x44,%ah
  44c2b8:	0f 8b 67 15 00 00    	jnp    0x44d825
  44c2be:	d9 05 48 79 4a 00    	flds   0x4a7948
  44c2c4:	83 ec 08             	sub    $0x8,%esp
  44c2c7:	d8 4f 30             	fmuls  0x30(%edi)
  44c2ca:	d9 5c 24 24          	fstps  0x24(%esp)
  44c2ce:	d9 44 24 24          	flds   0x24(%esp)
  44c2d2:	d9 5c 24 04          	fstps  0x4(%esp)
  44c2d6:	d9 47 24             	flds   0x24(%edi)
  44c2d9:	d9 1c 24             	fstps  (%esp)
  44c2dc:	e8 4f cb 00 00       	call   0x458e30
  44c2e1:	bb 04 00 00 00       	mov    $0x4,%ebx
  44c2e6:	d9 5f 24             	fstps  0x24(%edi)
  44c2e9:	09 9f 04 04 00 00    	or     %ebx,0x404(%edi)
  44c2ef:	e9 36 15 00 00       	jmp    0x44d82a
  44c2f4:	8b 4b 08             	mov    0x8(%ebx),%ecx
  44c2f7:	33 8f 04 04 00 00    	xor    0x404(%edi),%ecx
  44c2fd:	83 e1 01             	and    $0x1,%ecx
  44c300:	31 8f 04 04 00 00    	xor    %ecx,0x404(%edi)
  44c306:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44c30a:	03 cb                	add    %ebx,%ecx
  44c30c:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44c312:	e9 f2 f1 ff ff       	jmp    0x44b509
  44c317:	0f b7 53 08          	movzwl 0x8(%ebx),%edx
  44c31b:	c1 e2 12             	shl    $0x12,%edx
  44c31e:	33 97 04 04 00 00    	xor    0x404(%edi),%edx
  44c324:	81 e2 00 00 0c 00    	and    $0xc0000,%edx
  44c32a:	31 97 04 04 00 00    	xor    %edx,0x404(%edi)
  44c330:	0f b7 4b 0a          	movzwl 0xa(%ebx),%ecx
  44c334:	8b 87 04 04 00 00    	mov    0x404(%edi),%eax
  44c33a:	c1 e1 14             	shl    $0x14,%ecx
  44c33d:	33 c8                	xor    %eax,%ecx
  44c33f:	81 e1 00 00 30 00    	and    $0x300000,%ecx
  44c345:	33 c8                	xor    %eax,%ecx
  44c347:	89 8f 04 04 00 00    	mov    %ecx,0x404(%edi)
  44c34d:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44c351:	03 cb                	add    %ebx,%ecx
  44c353:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44c359:	e9 ab f1 ff ff       	jmp    0x44b509
  44c35e:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44c362:	d9 43 08             	flds   0x8(%ebx)
  44c365:	74 0b                	je     0x44c372
  44c367:	51                   	push   %ecx
  44c368:	8b c7                	mov    %edi,%eax
  44c36a:	d9 1c 24             	fstps  (%esp)
  44c36d:	e8 0e ed ff ff       	call   0x44b080
  44c372:	d9 5c 24 10          	fstps  0x10(%esp)
  44c376:	d9 44 24 10          	flds   0x10(%esp)
  44c37a:	d9 9f ac 02 00 00    	fstps  0x2ac(%edi)
  44c380:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44c384:	03 cb                	add    %ebx,%ecx
  44c386:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44c38c:	e9 78 f1 ff ff       	jmp    0x44b509
  44c391:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44c395:	d9 43 08             	flds   0x8(%ebx)
  44c398:	74 0b                	je     0x44c3a5
  44c39a:	51                   	push   %ecx
  44c39b:	8b c7                	mov    %edi,%eax
  44c39d:	d9 1c 24             	fstps  (%esp)
  44c3a0:	e8 db ec ff ff       	call   0x44b080
  44c3a5:	d9 5c 24 10          	fstps  0x10(%esp)
  44c3a9:	d9 44 24 10          	flds   0x10(%esp)
  44c3ad:	d9 9f b0 02 00 00    	fstps  0x2b0(%edi)
  44c3b3:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44c3b7:	03 cb                	add    %ebx,%ecx
  44c3b9:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44c3bf:	e9 45 f1 ff ff       	jmp    0x44b509
  44c3c4:	8b 53 08             	mov    0x8(%ebx),%edx
  44c3c7:	c1 e2 0b             	shl    $0xb,%edx
  44c3ca:	33 97 04 04 00 00    	xor    0x404(%edi),%edx
  44c3d0:	81 e2 00 08 00 00    	and    $0x800,%edx
  44c3d6:	31 97 04 04 00 00    	xor    %edx,0x404(%edi)
  44c3dc:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44c3e0:	03 cb                	add    %ebx,%ecx
  44c3e2:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44c3e8:	e9 1c f1 ff ff       	jmp    0x44b509
  44c3ed:	8b 43 08             	mov    0x8(%ebx),%eax
  44c3f0:	c1 e0 0d             	shl    $0xd,%eax
  44c3f3:	33 87 04 04 00 00    	xor    0x404(%edi),%eax
  44c3f9:	25 00 20 00 00       	and    $0x2000,%eax
  44c3fe:	e9 0b 14 00 00       	jmp    0x44d80e
  44c403:	8b 4b 08             	mov    0x8(%ebx),%ecx
  44c406:	8b 97 04 04 00 00    	mov    0x404(%edi),%edx
  44c40c:	c1 e1 1f             	shl    $0x1f,%ecx
  44c40f:	81 e2 ff ff ff 7f    	and    $0x7fffffff,%edx
  44c415:	0b ca                	or     %edx,%ecx
  44c417:	89 8f 04 04 00 00    	mov    %ecx,0x404(%edi)
  44c41d:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44c421:	03 cb                	add    %ebx,%ecx
  44c423:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44c429:	e9 db f0 ff ff       	jmp    0x44b509
  44c42e:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44c432:	8b 43 08             	mov    0x8(%ebx),%eax
  44c435:	74 07                	je     0x44c43e
  44c437:	8b cf                	mov    %edi,%ecx
  44c439:	e8 62 ee ff ff       	call   0x44b2a0
  44c43e:	f7 87 04 04 00 00 00 	testl  $0x100,0x404(%edi)
  44c445:	01 00 00 
  44c448:	89 87 d4 00 00 00    	mov    %eax,0xd4(%edi)
  44c44e:	a1 6c 32 4c 00       	mov    0x4c326c,%eax
  44c453:	89 87 a8 00 00 00    	mov    %eax,0xa8(%edi)
  44c459:	8b 0d 70 32 4c 00    	mov    0x4c3270,%ecx
  44c45f:	89 8f ac 00 00 00    	mov    %ecx,0xac(%edi)
  44c465:	8b 15 74 32 4c 00    	mov    0x4c3274,%edx
  44c46b:	89 97 b0 00 00 00    	mov    %edx,0xb0(%edi)
  44c471:	a1 6c 32 4c 00       	mov    0x4c326c,%eax
  44c476:	89 87 b4 00 00 00    	mov    %eax,0xb4(%edi)
  44c47c:	8b 0d 70 32 4c 00    	mov    0x4c3270,%ecx
  44c482:	89 8f b8 00 00 00    	mov    %ecx,0xb8(%edi)
  44c488:	8b 15 74 32 4c 00    	mov    0x4c3274,%edx
  44c48e:	89 97 bc 00 00 00    	mov    %edx,0xbc(%edi)
  44c494:	8b 43 0c             	mov    0xc(%ebx),%eax
  44c497:	89 87 d8 00 00 00    	mov    %eax,0xd8(%edi)
  44c49d:	8d b7 90 00 00 00    	lea    0x90(%edi),%esi
  44c4a3:	75 14                	jne    0x44c4b9
  44c4a5:	8b 8f dc 03 00 00    	mov    0x3dc(%edi),%ecx
  44c4ab:	8b 97 e0 03 00 00    	mov    0x3e0(%edi),%edx
  44c4b1:	8b 87 e4 03 00 00    	mov    0x3e4(%edi),%eax
  44c4b7:	eb 12                	jmp    0x44c4cb
  44c4b9:	8b 8f f4 03 00 00    	mov    0x3f4(%edi),%ecx
  44c4bf:	8b 97 f8 03 00 00    	mov    0x3f8(%edi),%edx
  44c4c5:	8b 87 fc 03 00 00    	mov    0x3fc(%edi),%eax
  44c4cb:	89 0e                	mov    %ecx,(%esi)
  44c4cd:	89 56 04             	mov    %edx,0x4(%esi)
  44c4d0:	89 46 08             	mov    %eax,0x8(%esi)
  44c4d3:	d9 43 18             	flds   0x18(%ebx)
  44c4d6:	f6 43 06 10          	testb  $0x10,0x6(%ebx)
  44c4da:	74 0b                	je     0x44c4e7
  44c4dc:	51                   	push   %ecx
  44c4dd:	8b c7                	mov    %edi,%eax
  44c4df:	d9 1c 24             	fstps  (%esp)
  44c4e2:	e8 99 eb ff ff       	call   0x44b080
  44c4e7:	f6 43 06 08          	testb  $0x8,0x6(%ebx)
  44c4eb:	d9 5c 24 18          	fstps  0x18(%esp)
  44c4ef:	d9 43 14             	flds   0x14(%ebx)
  44c4f2:	74 0b                	je     0x44c4ff
  44c4f4:	51                   	push   %ecx
  44c4f5:	8b c7                	mov    %edi,%eax
  44c4f7:	d9 1c 24             	fstps  (%esp)
  44c4fa:	e8 81 eb ff ff       	call   0x44b080
  44c4ff:	f6 43 06 04          	testb  $0x4,0x6(%ebx)
  44c503:	d9 5c 24 14          	fstps  0x14(%esp)
  44c507:	d9 43 10             	flds   0x10(%ebx)
  44c50a:	74 0b                	je     0x44c517
  44c50c:	51                   	push   %ecx
  44c50d:	8b c7                	mov    %edi,%eax
  44c50f:	d9 1c 24             	fstps  (%esp)
  44c512:	e8 69 eb ff ff       	call   0x44b080
  44c517:	d9 5c 24 10          	fstps  0x10(%esp)
  44c51b:	d9 44 24 10          	flds   0x10(%esp)
  44c51f:	d9 5c 24 50          	fstps  0x50(%esp)
  44c523:	8b 4c 24 50          	mov    0x50(%esp),%ecx
  44c527:	d9 44 24 14          	flds   0x14(%esp)
  44c52b:	89 8f 9c 00 00 00    	mov    %ecx,0x9c(%edi)
  44c531:	d9 5c 24 54          	fstps  0x54(%esp)
  44c535:	8b 54 24 54          	mov    0x54(%esp),%edx
  44c539:	d9 44 24 18          	flds   0x18(%esp)
  44c53d:	89 97 a0 00 00 00    	mov    %edx,0xa0(%edi)
  44c543:	d9 5c 24 58          	fstps  0x58(%esp)
  44c547:	8b 44 24 58          	mov    0x58(%esp),%eax
  44c54b:	89 87 a4 00 00 00    	mov    %eax,0xa4(%edi)
  44c551:	8b c6                	mov    %esi,%eax
  44c553:	e8 08 97 fb ff       	call   0x405c60
  44c558:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44c55c:	03 cb                	add    %ebx,%ecx
  44c55e:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44c564:	e9 a0 ef ff ff       	jmp    0x44b509
  44c569:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44c56d:	d9 43 0c             	flds   0xc(%ebx)
  44c570:	74 0b                	je     0x44c57d
  44c572:	51                   	push   %ecx
  44c573:	8b c7                	mov    %edi,%eax
  44c575:	d9 1c 24             	fstps  (%esp)
  44c578:	e8 03 eb ff ff       	call   0x44b080
  44c57d:	f6 43 06 04          	testb  $0x4,0x6(%ebx)
  44c581:	d9 5c 24 68          	fstps  0x68(%esp)
  44c585:	d9 43 10             	flds   0x10(%ebx)
  44c588:	74 0b                	je     0x44c595
  44c58a:	51                   	push   %ecx
  44c58b:	8b c7                	mov    %edi,%eax
  44c58d:	d9 1c 24             	fstps  (%esp)
  44c590:	e8 eb ea ff ff       	call   0x44b080
  44c595:	f6 43 06 08          	testb  $0x8,0x6(%ebx)
  44c599:	d9 5c 24 6c          	fstps  0x6c(%esp)
  44c59d:	d9 43 14             	flds   0x14(%ebx)
  44c5a0:	74 0b                	je     0x44c5ad
  44c5a2:	51                   	push   %ecx
  44c5a3:	8b c7                	mov    %edi,%eax
  44c5a5:	d9 1c 24             	fstps  (%esp)
  44c5a8:	e8 d3 ea ff ff       	call   0x44b080
  44c5ad:	f6 43 06 80          	testb  $0x80,0x6(%ebx)
  44c5b1:	d9 5c 24 70          	fstps  0x70(%esp)
  44c5b5:	d9 43 24             	flds   0x24(%ebx)
  44c5b8:	74 0b                	je     0x44c5c5
  44c5ba:	51                   	push   %ecx
  44c5bb:	8b c7                	mov    %edi,%eax
  44c5bd:	d9 1c 24             	fstps  (%esp)
  44c5c0:	e8 bb ea ff ff       	call   0x44b080
  44c5c5:	b9 00 01 00 00       	mov    $0x100,%ecx
  44c5ca:	d9 5c 24 78          	fstps  0x78(%esp)
  44c5ce:	d9 43 28             	flds   0x28(%ebx)
  44c5d1:	66 85 4b 06          	test   %cx,0x6(%ebx)
  44c5d5:	74 0b                	je     0x44c5e2
  44c5d7:	51                   	push   %ecx
  44c5d8:	8b c7                	mov    %edi,%eax
  44c5da:	d9 1c 24             	fstps  (%esp)
  44c5dd:	e8 9e ea ff ff       	call   0x44b080
  44c5e2:	ba 00 02 00 00       	mov    $0x200,%edx
  44c5e7:	d9 5c 24 7c          	fstps  0x7c(%esp)
  44c5eb:	d9 43 2c             	flds   0x2c(%ebx)
  44c5ee:	66 85 53 06          	test   %dx,0x6(%ebx)
  44c5f2:	74 0b                	je     0x44c5ff
  44c5f4:	51                   	push   %ecx
  44c5f5:	8b c7                	mov    %edi,%eax
  44c5f7:	d9 1c 24             	fstps  (%esp)
  44c5fa:	e8 81 ea ff ff       	call   0x44b080
  44c5ff:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44c603:	d9 9c 24 80 00 00 00 	fstps  0x80(%esp)
  44c60a:	8b 43 08             	mov    0x8(%ebx),%eax
  44c60d:	74 07                	je     0x44c616
  44c60f:	8b cf                	mov    %edi,%ecx
  44c611:	e8 8a ec ff ff       	call   0x44b2a0
  44c616:	f7 87 04 04 00 00 00 	testl  $0x100,0x404(%edi)
  44c61d:	01 00 00 
  44c620:	8b 4c 24 6c          	mov    0x6c(%esp),%ecx
  44c624:	8b 54 24 70          	mov    0x70(%esp),%edx
  44c628:	89 87 d4 00 00 00    	mov    %eax,0xd4(%edi)
  44c62e:	8b 44 24 68          	mov    0x68(%esp),%eax
  44c632:	89 87 a8 00 00 00    	mov    %eax,0xa8(%edi)
  44c638:	8b 44 24 78          	mov    0x78(%esp),%eax
  44c63c:	89 8f ac 00 00 00    	mov    %ecx,0xac(%edi)
  44c642:	8b 4c 24 7c          	mov    0x7c(%esp),%ecx
  44c646:	89 97 b0 00 00 00    	mov    %edx,0xb0(%edi)
  44c64c:	8b 94 24 80 00 00 00 	mov    0x80(%esp),%edx
  44c653:	89 87 b4 00 00 00    	mov    %eax,0xb4(%edi)
  44c659:	89 8f b8 00 00 00    	mov    %ecx,0xb8(%edi)
  44c65f:	89 97 bc 00 00 00    	mov    %edx,0xbc(%edi)
  44c665:	c7 87 d8 00 00 00 08 	movl   $0x8,0xd8(%edi)
  44c66c:	00 00 00 
  44c66f:	8d b7 90 00 00 00    	lea    0x90(%edi),%esi
  44c675:	75 14                	jne    0x44c68b
  44c677:	8b 87 dc 03 00 00    	mov    0x3dc(%edi),%eax
  44c67d:	8b 8f e0 03 00 00    	mov    0x3e0(%edi),%ecx
  44c683:	8b 97 e4 03 00 00    	mov    0x3e4(%edi),%edx
  44c689:	eb 12                	jmp    0x44c69d
  44c68b:	8b 87 f4 03 00 00    	mov    0x3f4(%edi),%eax
  44c691:	8b 8f f8 03 00 00    	mov    0x3f8(%edi),%ecx
  44c697:	8b 97 fc 03 00 00    	mov    0x3fc(%edi),%edx
  44c69d:	89 06                	mov    %eax,(%esi)
  44c69f:	89 4e 04             	mov    %ecx,0x4(%esi)
  44c6a2:	89 56 08             	mov    %edx,0x8(%esi)
  44c6a5:	d9 43 20             	flds   0x20(%ebx)
  44c6a8:	f6 43 06 40          	testb  $0x40,0x6(%ebx)
  44c6ac:	74 0b                	je     0x44c6b9
  44c6ae:	51                   	push   %ecx
  44c6af:	8b c7                	mov    %edi,%eax
  44c6b1:	d9 1c 24             	fstps  (%esp)
  44c6b4:	e8 c7 e9 ff ff       	call   0x44b080
  44c6b9:	f6 43 06 20          	testb  $0x20,0x6(%ebx)
  44c6bd:	d9 5c 24 18          	fstps  0x18(%esp)
  44c6c1:	d9 43 1c             	flds   0x1c(%ebx)
  44c6c4:	74 0b                	je     0x44c6d1
  44c6c6:	51                   	push   %ecx
  44c6c7:	8b c7                	mov    %edi,%eax
  44c6c9:	d9 1c 24             	fstps  (%esp)
  44c6cc:	e8 af e9 ff ff       	call   0x44b080
  44c6d1:	f6 43 06 10          	testb  $0x10,0x6(%ebx)
  44c6d5:	d9 5c 24 14          	fstps  0x14(%esp)
  44c6d9:	d9 43 18             	flds   0x18(%ebx)
  44c6dc:	74 0b                	je     0x44c6e9
  44c6de:	51                   	push   %ecx
  44c6df:	8b c7                	mov    %edi,%eax
  44c6e1:	d9 1c 24             	fstps  (%esp)
  44c6e4:	e8 97 e9 ff ff       	call   0x44b080
  44c6e9:	d9 5c 24 10          	fstps  0x10(%esp)
  44c6ed:	d9 44 24 10          	flds   0x10(%esp)
  44c6f1:	d9 5c 24 30          	fstps  0x30(%esp)
  44c6f5:	8b 44 24 30          	mov    0x30(%esp),%eax
  44c6f9:	d9 44 24 14          	flds   0x14(%esp)
  44c6fd:	89 87 9c 00 00 00    	mov    %eax,0x9c(%edi)
  44c703:	d9 5c 24 34          	fstps  0x34(%esp)
  44c707:	8b 4c 24 34          	mov    0x34(%esp),%ecx
  44c70b:	d9 44 24 18          	flds   0x18(%esp)
  44c70f:	89 8f a0 00 00 00    	mov    %ecx,0xa0(%edi)
  44c715:	d9 5c 24 38          	fstps  0x38(%esp)
  44c719:	8b 54 24 38          	mov    0x38(%esp),%edx
  44c71d:	8b c6                	mov    %esi,%eax
  44c71f:	89 97 a4 00 00 00    	mov    %edx,0xa4(%edi)
  44c725:	e8 36 95 fb ff       	call   0x405c60
  44c72a:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44c72e:	03 cb                	add    %ebx,%ecx
  44c730:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44c736:	e9 ce ed ff ff       	jmp    0x44b509
  44c73b:	f6 43 06 10          	testb  $0x10,0x6(%ebx)
  44c73f:	8a 87 74 03 00 00    	mov    0x374(%edi),%al
  44c745:	8a 8f 75 03 00 00    	mov    0x375(%edi),%cl
  44c74b:	8a 97 76 03 00 00    	mov    0x376(%edi),%dl
  44c751:	88 44 24 64          	mov    %al,0x64(%esp)
  44c755:	8b 43 18             	mov    0x18(%ebx),%eax
  44c758:	88 4c 24 65          	mov    %cl,0x65(%esp)
  44c75c:	88 54 24 66          	mov    %dl,0x66(%esp)
  44c760:	74 07                	je     0x44c769
  44c762:	8b cf                	mov    %edi,%ecx
  44c764:	e8 37 eb ff ff       	call   0x44b2a0
  44c769:	f6 43 06 08          	testb  $0x8,0x6(%ebx)
  44c76d:	89 44 24 10          	mov    %eax,0x10(%esp)
  44c771:	74 10                	je     0x44c783
  44c773:	8b 43 14             	mov    0x14(%ebx),%eax
  44c776:	8b cf                	mov    %edi,%ecx
  44c778:	e8 23 eb ff ff       	call   0x44b2a0
  44c77d:	89 44 24 14          	mov    %eax,0x14(%esp)
  44c781:	eb 07                	jmp    0x44c78a
  44c783:	8b 4b 14             	mov    0x14(%ebx),%ecx
  44c786:	89 4c 24 14          	mov    %ecx,0x14(%esp)
  44c78a:	f6 43 06 04          	testb  $0x4,0x6(%ebx)
  44c78e:	8b 43 10             	mov    0x10(%ebx),%eax
  44c791:	74 07                	je     0x44c79a
  44c793:	8b cf                	mov    %edi,%ecx
  44c795:	e8 06 eb ff ff       	call   0x44b2a0
  44c79a:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44c79e:	8a 54 24 10          	mov    0x10(%esp),%dl
  44c7a2:	8a 4c 24 14          	mov    0x14(%esp),%cl
  44c7a6:	88 44 24 5e          	mov    %al,0x5e(%esp)
  44c7aa:	8b 43 08             	mov    0x8(%ebx),%eax
  44c7ad:	88 54 24 5c          	mov    %dl,0x5c(%esp)
  44c7b1:	88 4c 24 5d          	mov    %cl,0x5d(%esp)
  44c7b5:	74 07                	je     0x44c7be
  44c7b7:	8b cf                	mov    %edi,%ecx
  44c7b9:	e8 e2 ea ff ff       	call   0x44b2a0
  44c7be:	0f b6 53 0c          	movzbl 0xc(%ebx),%edx
  44c7c2:	52                   	push   %edx
  44c7c3:	50                   	push   %eax
  44c7c4:	8d 4c 24 64          	lea    0x64(%esp),%ecx
  44c7c8:	8d 54 24 6c          	lea    0x6c(%esp),%edx
  44c7cc:	8b c7                	mov    %edi,%eax
  44c7ce:	e8 9d 2a 00 00       	call   0x44f270
  44c7d3:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44c7d7:	03 cb                	add    %ebx,%ecx
  44c7d9:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44c7df:	e9 25 ed ff ff       	jmp    0x44b509
  44c7e4:	f6 43 06 04          	testb  $0x4,0x6(%ebx)
  44c7e8:	74 0e                	je     0x44c7f8
  44c7ea:	8b 43 10             	mov    0x10(%ebx),%eax
  44c7ed:	8b cf                	mov    %edi,%ecx
  44c7ef:	e8 ac ea ff ff       	call   0x44b2a0
  44c7f4:	8b f0                	mov    %eax,%esi
  44c7f6:	eb 03                	jmp    0x44c7fb
  44c7f8:	8b 73 10             	mov    0x10(%ebx),%esi
  44c7fb:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44c7ff:	8b 43 08             	mov    0x8(%ebx),%eax
  44c802:	74 07                	je     0x44c80b
  44c804:	8b cf                	mov    %edi,%ecx
  44c806:	e8 95 ea ff ff       	call   0x44b2a0
  44c80b:	0f b6 8f 77 03 00 00 	movzbl 0x377(%edi),%ecx
  44c812:	0f b6 53 0c          	movzbl 0xc(%ebx),%edx
  44c816:	56                   	push   %esi
  44c817:	51                   	push   %ecx
  44c818:	8b c8                	mov    %eax,%ecx
  44c81a:	52                   	push   %edx
  44c81b:	8b c7                	mov    %edi,%eax
  44c81d:	e8 de 2b 00 00       	call   0x44f400
  44c822:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44c826:	03 cb                	add    %ebx,%ecx
  44c828:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44c82e:	e9 d6 ec ff ff       	jmp    0x44b509
  44c833:	f6 43 06 10          	testb  $0x10,0x6(%ebx)
  44c837:	8a 87 78 03 00 00    	mov    0x378(%edi),%al
  44c83d:	8a 8f 79 03 00 00    	mov    0x379(%edi),%cl
  44c843:	8a 97 7a 03 00 00    	mov    0x37a(%edi),%dl
  44c849:	88 44 24 20          	mov    %al,0x20(%esp)
  44c84d:	8b 43 18             	mov    0x18(%ebx),%eax
  44c850:	88 4c 24 21          	mov    %cl,0x21(%esp)
  44c854:	88 54 24 22          	mov    %dl,0x22(%esp)
  44c858:	74 07                	je     0x44c861
  44c85a:	8b cf                	mov    %edi,%ecx
  44c85c:	e8 3f ea ff ff       	call   0x44b2a0
  44c861:	f6 43 06 08          	testb  $0x8,0x6(%ebx)
  44c865:	89 44 24 10          	mov    %eax,0x10(%esp)
  44c869:	74 10                	je     0x44c87b
  44c86b:	8b 43 14             	mov    0x14(%ebx),%eax
  44c86e:	8b cf                	mov    %edi,%ecx
  44c870:	e8 2b ea ff ff       	call   0x44b2a0
  44c875:	89 44 24 14          	mov    %eax,0x14(%esp)
  44c879:	eb 07                	jmp    0x44c882
  44c87b:	8b 4b 14             	mov    0x14(%ebx),%ecx
  44c87e:	89 4c 24 14          	mov    %ecx,0x14(%esp)
  44c882:	f6 43 06 04          	testb  $0x4,0x6(%ebx)
  44c886:	8b 43 10             	mov    0x10(%ebx),%eax
  44c889:	74 07                	je     0x44c892
  44c88b:	8b cf                	mov    %edi,%ecx
  44c88d:	e8 0e ea ff ff       	call   0x44b2a0
  44c892:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44c896:	8a 54 24 10          	mov    0x10(%esp),%dl
  44c89a:	8a 4c 24 14          	mov    0x14(%esp),%cl
  44c89e:	88 44 24 62          	mov    %al,0x62(%esp)
  44c8a2:	8b 43 08             	mov    0x8(%ebx),%eax
  44c8a5:	88 54 24 60          	mov    %dl,0x60(%esp)
  44c8a9:	88 4c 24 61          	mov    %cl,0x61(%esp)
  44c8ad:	74 07                	je     0x44c8b6
  44c8af:	8b cf                	mov    %edi,%ecx
  44c8b1:	e8 ea e9 ff ff       	call   0x44b2a0
  44c8b6:	0f b6 53 0c          	movzbl 0xc(%ebx),%edx
  44c8ba:	52                   	push   %edx
  44c8bb:	50                   	push   %eax
  44c8bc:	8d 4c 24 68          	lea    0x68(%esp),%ecx
  44c8c0:	8d 54 24 28          	lea    0x28(%esp),%edx
  44c8c4:	8b c7                	mov    %edi,%eax
  44c8c6:	e8 b5 27 00 00       	call   0x44f080
  44c8cb:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44c8cf:	03 cb                	add    %ebx,%ecx
  44c8d1:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44c8d7:	e9 2d ec ff ff       	jmp    0x44b509
  44c8dc:	f6 43 06 04          	testb  $0x4,0x6(%ebx)
  44c8e0:	74 0e                	je     0x44c8f0
  44c8e2:	8b 43 10             	mov    0x10(%ebx),%eax
  44c8e5:	8b cf                	mov    %edi,%ecx
  44c8e7:	e8 b4 e9 ff ff       	call   0x44b2a0
  44c8ec:	8b f0                	mov    %eax,%esi
  44c8ee:	eb 03                	jmp    0x44c8f3
  44c8f0:	8b 73 10             	mov    0x10(%ebx),%esi
  44c8f3:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44c8f7:	8b 43 08             	mov    0x8(%ebx),%eax
  44c8fa:	74 07                	je     0x44c903
  44c8fc:	8b cf                	mov    %edi,%ecx
  44c8fe:	e8 9d e9 ff ff       	call   0x44b2a0
  44c903:	0f b6 8f 7b 03 00 00 	movzbl 0x37b(%edi),%ecx
  44c90a:	8a 53 0c             	mov    0xc(%ebx),%dl
  44c90d:	56                   	push   %esi
  44c90e:	51                   	push   %ecx
  44c90f:	8b c8                	mov    %eax,%ecx
  44c911:	8b c7                	mov    %edi,%eax
  44c913:	e8 e8 26 00 00       	call   0x44f000
  44c918:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44c91c:	03 cb                	add    %ebx,%ecx
  44c91e:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44c924:	e9 e0 eb ff ff       	jmp    0x44b509
  44c929:	f6 43 06 10          	testb  $0x10,0x6(%ebx)
  44c92d:	d9 43 18             	flds   0x18(%ebx)
  44c930:	74 0b                	je     0x44c93d
  44c932:	51                   	push   %ecx
  44c933:	8b c7                	mov    %edi,%eax
  44c935:	d9 1c 24             	fstps  (%esp)
  44c938:	e8 43 e7 ff ff       	call   0x44b080
  44c93d:	f6 43 06 08          	testb  $0x8,0x6(%ebx)
  44c941:	d9 5c 24 18          	fstps  0x18(%esp)
  44c945:	d9 43 14             	flds   0x14(%ebx)
  44c948:	74 0b                	je     0x44c955
  44c94a:	51                   	push   %ecx
  44c94b:	8b c7                	mov    %edi,%eax
  44c94d:	d9 1c 24             	fstps  (%esp)
  44c950:	e8 2b e7 ff ff       	call   0x44b080
  44c955:	f6 43 06 04          	testb  $0x4,0x6(%ebx)
  44c959:	d9 5c 24 14          	fstps  0x14(%esp)
  44c95d:	d9 43 10             	flds   0x10(%ebx)
  44c960:	74 0b                	je     0x44c96d
  44c962:	51                   	push   %ecx
  44c963:	8b c7                	mov    %edi,%eax
  44c965:	d9 1c 24             	fstps  (%esp)
  44c968:	e8 13 e7 ff ff       	call   0x44b080
  44c96d:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44c971:	d9 5c 24 10          	fstps  0x10(%esp)
  44c975:	d9 44 24 10          	flds   0x10(%esp)
  44c979:	8b 43 08             	mov    0x8(%ebx),%eax
  44c97c:	d9 5c 24 24          	fstps  0x24(%esp)
  44c980:	d9 44 24 14          	flds   0x14(%esp)
  44c984:	d9 5c 24 28          	fstps  0x28(%esp)
  44c988:	d9 44 24 18          	flds   0x18(%esp)
  44c98c:	d9 5c 24 2c          	fstps  0x2c(%esp)
  44c990:	74 07                	je     0x44c999
  44c992:	8b cf                	mov    %edi,%ecx
  44c994:	e8 07 e9 ff ff       	call   0x44b2a0
  44c999:	89 87 98 01 00 00    	mov    %eax,0x198(%edi)
  44c99f:	8b 15 6c 32 4c 00    	mov    0x4c326c,%edx
  44c9a5:	89 97 6c 01 00 00    	mov    %edx,0x16c(%edi)
  44c9ab:	a1 70 32 4c 00       	mov    0x4c3270,%eax
  44c9b0:	89 87 70 01 00 00    	mov    %eax,0x170(%edi)
  44c9b6:	8b 0d 74 32 4c 00    	mov    0x4c3274,%ecx
  44c9bc:	89 8f 74 01 00 00    	mov    %ecx,0x174(%edi)
  44c9c2:	8b 15 6c 32 4c 00    	mov    0x4c326c,%edx
  44c9c8:	89 97 78 01 00 00    	mov    %edx,0x178(%edi)
  44c9ce:	a1 70 32 4c 00       	mov    0x4c3270,%eax
  44c9d3:	89 87 7c 01 00 00    	mov    %eax,0x17c(%edi)
  44c9d9:	8b 0d 74 32 4c 00    	mov    0x4c3274,%ecx
  44c9df:	89 8f 80 01 00 00    	mov    %ecx,0x180(%edi)
  44c9e5:	8b 53 0c             	mov    0xc(%ebx),%edx
  44c9e8:	8b 4f 24             	mov    0x24(%edi),%ecx
  44c9eb:	89 97 9c 01 00 00    	mov    %edx,0x19c(%edi)
  44c9f1:	8b 57 28             	mov    0x28(%edi),%edx
  44c9f4:	8d 87 54 01 00 00    	lea    0x154(%edi),%eax
  44c9fa:	89 08                	mov    %ecx,(%eax)
  44c9fc:	8b 4f 2c             	mov    0x2c(%edi),%ecx
  44c9ff:	89 50 04             	mov    %edx,0x4(%eax)
  44ca02:	8b 54 24 24          	mov    0x24(%esp),%edx
  44ca06:	89 97 60 01 00 00    	mov    %edx,0x160(%edi)
  44ca0c:	8b 54 24 2c          	mov    0x2c(%esp),%edx
  44ca10:	89 48 08             	mov    %ecx,0x8(%eax)
  44ca13:	8b 4c 24 28          	mov    0x28(%esp),%ecx
  44ca17:	89 8f 64 01 00 00    	mov    %ecx,0x164(%edi)
  44ca1d:	89 97 68 01 00 00    	mov    %edx,0x168(%edi)
  44ca23:	e8 38 92 fb ff       	call   0x405c60
  44ca28:	83 8f 04 04 00 00 04 	orl    $0x4,0x404(%edi)
  44ca2f:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44ca33:	03 cb                	add    %ebx,%ecx
  44ca35:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44ca3b:	e9 c9 ea ff ff       	jmp    0x44b509
  44ca40:	f6 43 06 08          	testb  $0x8,0x6(%ebx)
  44ca44:	d9 43 14             	flds   0x14(%ebx)
  44ca47:	74 0b                	je     0x44ca54
  44ca49:	51                   	push   %ecx
  44ca4a:	8b c7                	mov    %edi,%eax
  44ca4c:	d9 1c 24             	fstps  (%esp)
  44ca4f:	e8 2c e6 ff ff       	call   0x44b080
  44ca54:	f6 43 06 04          	testb  $0x4,0x6(%ebx)
  44ca58:	d9 5c 24 14          	fstps  0x14(%esp)
  44ca5c:	d9 43 10             	flds   0x10(%ebx)
  44ca5f:	74 0b                	je     0x44ca6c
  44ca61:	51                   	push   %ecx
  44ca62:	8b c7                	mov    %edi,%eax
  44ca64:	d9 1c 24             	fstps  (%esp)
  44ca67:	e8 14 e6 ff ff       	call   0x44b080
  44ca6c:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44ca70:	d9 5c 24 10          	fstps  0x10(%esp)
  44ca74:	d9 44 24 10          	flds   0x10(%esp)
  44ca78:	8b 43 08             	mov    0x8(%ebx),%eax
  44ca7b:	d9 9c 24 88 00 00 00 	fstps  0x88(%esp)
  44ca82:	d9 44 24 14          	flds   0x14(%esp)
  44ca86:	d9 9c 24 8c 00 00 00 	fstps  0x8c(%esp)
  44ca8d:	74 07                	je     0x44ca96
  44ca8f:	8b cf                	mov    %edi,%ecx
  44ca91:	e8 0a e8 ff ff       	call   0x44b2a0
  44ca96:	0f b6 4b 0c          	movzbl 0xc(%ebx),%ecx
  44ca9a:	51                   	push   %ecx
  44ca9b:	50                   	push   %eax
  44ca9c:	8d 57 3c             	lea    0x3c(%edi),%edx
  44ca9f:	8d 8c 24 90 00 00 00 	lea    0x90(%esp),%ecx
  44caa6:	8b c7                	mov    %edi,%eax
  44caa8:	e8 73 66 fd ff       	call   0x423120
  44caad:	83 8f 04 04 00 00 08 	orl    $0x8,0x404(%edi)
  44cab4:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44cab8:	03 cb                	add    %ebx,%ecx
  44caba:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44cac0:	e9 44 ea ff ff       	jmp    0x44b509
  44cac5:	f6 43 06 04          	testb  $0x4,0x6(%ebx)
  44cac9:	d9 43 10             	flds   0x10(%ebx)
  44cacc:	74 0b                	je     0x44cad9
  44cace:	51                   	push   %ecx
  44cacf:	8b c7                	mov    %edi,%eax
  44cad1:	d9 1c 24             	fstps  (%esp)
  44cad4:	e8 a7 e5 ff ff       	call   0x44b080
  44cad9:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44cadd:	d9 5c 24 10          	fstps  0x10(%esp)
  44cae1:	8b 43 08             	mov    0x8(%ebx),%eax
  44cae4:	74 07                	je     0x44caed
  44cae6:	8b cf                	mov    %edi,%ecx
  44cae8:	e8 b3 e7 ff ff       	call   0x44b2a0
  44caed:	d9 ee                	fldz
  44caef:	89 87 78 02 00 00    	mov    %eax,0x278(%edi)
  44caf5:	d9 97 5c 02 00 00    	fsts   0x25c(%edi)
  44cafb:	8d 87 54 02 00 00    	lea    0x254(%edi),%eax
  44cb01:	d9 9f 60 02 00 00    	fstps  0x260(%edi)
  44cb07:	8b 53 0c             	mov    0xc(%ebx),%edx
  44cb0a:	d9 87 ac 02 00 00    	flds   0x2ac(%edi)
  44cb10:	d9 18                	fstps  (%eax)
  44cb12:	89 97 7c 02 00 00    	mov    %edx,0x27c(%edi)
  44cb18:	d9 44 24 10          	flds   0x10(%esp)
  44cb1c:	d9 9f 58 02 00 00    	fstps  0x258(%edi)
  44cb22:	e8 59 22 00 00       	call   0x44ed80
  44cb27:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44cb2b:	03 cb                	add    %ebx,%ecx
  44cb2d:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44cb33:	e9 d1 e9 ff ff       	jmp    0x44b509
  44cb38:	f6 43 06 04          	testb  $0x4,0x6(%ebx)
  44cb3c:	d9 43 10             	flds   0x10(%ebx)
  44cb3f:	74 0b                	je     0x44cb4c
  44cb41:	51                   	push   %ecx
  44cb42:	8b c7                	mov    %edi,%eax
  44cb44:	d9 1c 24             	fstps  (%esp)
  44cb47:	e8 34 e5 ff ff       	call   0x44b080
  44cb4c:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44cb50:	d9 5c 24 10          	fstps  0x10(%esp)
  44cb54:	8b 43 08             	mov    0x8(%ebx),%eax
  44cb57:	74 07                	je     0x44cb60
  44cb59:	8b cf                	mov    %edi,%ecx
  44cb5b:	e8 40 e7 ff ff       	call   0x44b2a0
  44cb60:	d9 ee                	fldz
  44cb62:	89 87 a4 02 00 00    	mov    %eax,0x2a4(%edi)
  44cb68:	d9 97 88 02 00 00    	fsts   0x288(%edi)
  44cb6e:	d9 9f 8c 02 00 00    	fstps  0x28c(%edi)
  44cb74:	8b 43 0c             	mov    0xc(%ebx),%eax
  44cb77:	d9 87 b0 02 00 00    	flds   0x2b0(%edi)
  44cb7d:	89 87 a8 02 00 00    	mov    %eax,0x2a8(%edi)
  44cb83:	d9 9f 80 02 00 00    	fstps  0x280(%edi)
  44cb89:	d9 44 24 10          	flds   0x10(%esp)
  44cb8d:	8d 87 80 02 00 00    	lea    0x280(%edi),%eax
  44cb93:	d9 9f 84 02 00 00    	fstps  0x284(%edi)
  44cb99:	e8 e2 21 00 00       	call   0x44ed80
  44cb9e:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44cba2:	03 cb                	add    %ebx,%ecx
  44cba4:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44cbaa:	e9 5a e9 ff ff       	jmp    0x44b509
  44cbaf:	8b 4b 08             	mov    0x8(%ebx),%ecx
  44cbb2:	c1 e1 16             	shl    $0x16,%ecx
  44cbb5:	33 8f 04 04 00 00    	xor    0x404(%edi),%ecx
  44cbbb:	81 e1 00 00 c0 03    	and    $0x3c00000,%ecx
  44cbc1:	31 8f 04 04 00 00    	xor    %ecx,0x404(%edi)
  44cbc7:	8b 87 04 04 00 00    	mov    0x404(%edi),%eax
  44cbcd:	25 00 00 c0 03       	and    $0x3c00000,%eax
  44cbd2:	3d 00 00 80 02       	cmp    $0x2800000,%eax
  44cbd7:	0f 85 37 0c 00 00    	jne    0x44d814
  44cbdd:	8b df                	mov    %edi,%ebx
  44cbdf:	e8 0c 55 00 00       	call   0x4520f0
  44cbe4:	8b 5c 24 18          	mov    0x18(%esp),%ebx
  44cbe8:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44cbec:	03 cb                	add    %ebx,%ecx
  44cbee:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44cbf4:	e9 10 e9 ff ff       	jmp    0x44b509
  44cbf9:	8b 97 e8 03 00 00    	mov    0x3e8(%edi),%edx
  44cbff:	d9 ee                	fldz
  44cc01:	8b 8f f0 03 00 00    	mov    0x3f0(%edi),%ecx
  44cc07:	8b 87 ec 03 00 00    	mov    0x3ec(%edi),%eax
  44cc0d:	d9 97 e8 03 00 00    	fsts   0x3e8(%edi)
  44cc13:	89 97 dc 03 00 00    	mov    %edx,0x3dc(%edi)
  44cc19:	d9 97 ec 03 00 00    	fsts   0x3ec(%edi)
  44cc1f:	89 87 e0 03 00 00    	mov    %eax,0x3e0(%edi)
  44cc25:	d9 9f f0 03 00 00    	fstps  0x3f0(%edi)
  44cc2b:	89 8f e4 03 00 00    	mov    %ecx,0x3e4(%edi)
  44cc31:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44cc35:	03 cb                	add    %ebx,%ecx
  44cc37:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44cc3d:	e9 c7 e8 ff ff       	jmp    0x44b509
  44cc42:	8b 97 04 04 00 00    	mov    0x404(%edi),%edx
  44cc48:	81 e2 ff ff 7f fe    	and    $0xfe7fffff,%edx
  44cc4e:	81 ca 00 00 40 02    	or     $0x2400000,%edx
  44cc54:	89 97 04 04 00 00    	mov    %edx,0x404(%edi)
  44cc5a:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44cc5e:	8b 43 08             	mov    0x8(%ebx),%eax
  44cc61:	74 07                	je     0x44cc6a
  44cc63:	8b cf                	mov    %edi,%ecx
  44cc65:	e8 36 e6 ff ff       	call   0x44b2a0
  44cc6a:	8d 0c c5 00 00 00 00 	lea    0x0(,%eax,8),%ecx
  44cc71:	2b c8                	sub    %eax,%ecx
  44cc73:	03 c9                	add    %ecx,%ecx
  44cc75:	03 c9                	add    %ecx,%ecx
  44cc77:	03 c9                	add    %ecx,%ecx
  44cc79:	51                   	push   %ecx
  44cc7a:	e8 13 35 01 00       	call   0x460192
  44cc7f:	89 87 00 04 00 00    	mov    %eax,0x400(%edi)
  44cc85:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44cc89:	83 c4 04             	add    $0x4,%esp
  44cc8c:	03 cb                	add    %ebx,%ecx
  44cc8e:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44cc94:	e9 70 e8 ff ff       	jmp    0x44b509
  44cc99:	8b 97 04 04 00 00    	mov    0x404(%edi),%edx
  44cc9f:	81 e2 ff ff 7f ff    	and    $0xff7fffff,%edx
  44cca5:	81 ca 00 00 40 03    	or     $0x3400000,%edx
  44ccab:	89 97 04 04 00 00    	mov    %edx,0x404(%edi)
  44ccb1:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44ccb5:	8b 43 08             	mov    0x8(%ebx),%eax
  44ccb8:	74 07                	je     0x44ccc1
  44ccba:	8b cf                	mov    %edi,%ecx
  44ccbc:	e8 df e5 ff ff       	call   0x44b2a0
  44ccc1:	8d 0c c5 00 00 00 00 	lea    0x0(,%eax,8),%ecx
  44ccc8:	2b c8                	sub    %eax,%ecx
  44ccca:	03 c9                	add    %ecx,%ecx
  44cccc:	03 c9                	add    %ecx,%ecx
  44ccce:	03 c9                	add    %ecx,%ecx
  44ccd0:	51                   	push   %ecx
  44ccd1:	e8 bc 34 01 00       	call   0x460192
  44ccd6:	89 87 00 04 00 00    	mov    %eax,0x400(%edi)
  44ccdc:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44cce0:	83 c4 04             	add    $0x4,%esp
  44cce3:	03 cb                	add    %ebx,%ecx
  44cce5:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44cceb:	e9 19 e8 ff ff       	jmp    0x44b509
  44ccf0:	8d 8c 24 a8 00 00 00 	lea    0xa8(%esp),%ecx
  44ccf7:	8b c7                	mov    %edi,%eax
  44ccf9:	e8 12 53 00 00       	call   0x452010
  44ccfe:	8d 4f 70             	lea    0x70(%edi),%ecx
  44cd01:	8d 84 24 a8 00 00 00 	lea    0xa8(%esp),%eax
  44cd08:	e8 63 e7 ff ff       	call   0x44b470
  44cd0d:	8d 4f 78             	lea    0x78(%edi),%ecx
  44cd10:	8d 84 24 b4 00 00 00 	lea    0xb4(%esp),%eax
  44cd17:	e8 54 e7 ff ff       	call   0x44b470
  44cd1c:	8d 8f 80 00 00 00    	lea    0x80(%edi),%ecx
  44cd22:	8d 84 24 c0 00 00 00 	lea    0xc0(%esp),%eax
  44cd29:	e8 42 e7 ff ff       	call   0x44b470
  44cd2e:	8d 8f 88 00 00 00    	lea    0x88(%edi),%ecx
  44cd34:	8d 84 24 cc 00 00 00 	lea    0xcc(%esp),%eax
  44cd3b:	e8 30 e7 ff ff       	call   0x44b470
  44cd40:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44cd44:	03 cb                	add    %ebx,%ecx
  44cd46:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44cd4c:	e9 b8 e7 ff ff       	jmp    0x44b509
  44cd51:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44cd55:	8b 43 08             	mov    0x8(%ebx),%eax
  44cd58:	74 07                	je     0x44cd61
  44cd5a:	8b cf                	mov    %edi,%ecx
  44cd5c:	e8 3f e5 ff ff       	call   0x44b2a0
  44cd61:	8d 14 85 00 00 00 00 	lea    0x0(,%eax,4),%edx
  44cd68:	33 97 08 04 00 00    	xor    0x408(%edi),%edx
  44cd6e:	83 e2 04             	and    $0x4,%edx
  44cd71:	31 97 08 04 00 00    	xor    %edx,0x408(%edi)
  44cd77:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44cd7b:	03 cb                	add    %ebx,%ecx
  44cd7d:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44cd83:	e9 81 e7 ff ff       	jmp    0x44b509
  44cd88:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44cd8c:	74 0e                	je     0x44cd9c
  44cd8e:	8b 43 0c             	mov    0xc(%ebx),%eax
  44cd91:	8b cf                	mov    %edi,%ecx
  44cd93:	e8 08 e5 ff ff       	call   0x44b2a0
  44cd98:	8b f0                	mov    %eax,%esi
  44cd9a:	eb 03                	jmp    0x44cd9f
  44cd9c:	8b 73 0c             	mov    0xc(%ebx),%esi
  44cd9f:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44cda3:	8d 43 08             	lea    0x8(%ebx),%eax
  44cda6:	0f 84 40 07 00 00    	je     0x44d4ec
  44cdac:	8b d7                	mov    %edi,%edx
  44cdae:	e8 4d e6 ff ff       	call   0x44b400
  44cdb3:	89 30                	mov    %esi,(%eax)
  44cdb5:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44cdb9:	03 cb                	add    %ebx,%ecx
  44cdbb:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44cdc1:	e9 43 e7 ff ff       	jmp    0x44b509
  44cdc6:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44cdca:	d9 43 0c             	flds   0xc(%ebx)
  44cdcd:	74 0b                	je     0x44cdda
  44cdcf:	51                   	push   %ecx
  44cdd0:	8b c7                	mov    %edi,%eax
  44cdd2:	d9 1c 24             	fstps  (%esp)
  44cdd5:	e8 a6 e2 ff ff       	call   0x44b080
  44cdda:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44cdde:	d9 5c 24 10          	fstps  0x10(%esp)
  44cde2:	8d 43 08             	lea    0x8(%ebx),%eax
  44cde5:	74 10                	je     0x44cdf7
  44cde7:	8b 75 08             	mov    0x8(%ebp),%esi
  44cdea:	8b f8                	mov    %eax,%edi
  44cdec:	e8 8f e5 ff ff       	call   0x44b380
  44cdf1:	8b 5c 24 18          	mov    0x18(%esp),%ebx
  44cdf5:	8b fe                	mov    %esi,%edi
  44cdf7:	d9 44 24 10          	flds   0x10(%esp)
  44cdfb:	d9 18                	fstps  (%eax)
  44cdfd:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44ce01:	03 cb                	add    %ebx,%ecx
  44ce03:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44ce09:	e9 fb e6 ff ff       	jmp    0x44b509
  44ce0e:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44ce12:	8b 43 0c             	mov    0xc(%ebx),%eax
  44ce15:	74 07                	je     0x44ce1e
  44ce17:	8b cf                	mov    %edi,%ecx
  44ce19:	e8 82 e4 ff ff       	call   0x44b2a0
  44ce1e:	f6 43 06 04          	testb  $0x4,0x6(%ebx)
  44ce22:	89 44 24 10          	mov    %eax,0x10(%esp)
  44ce26:	74 0e                	je     0x44ce36
  44ce28:	8b 43 10             	mov    0x10(%ebx),%eax
  44ce2b:	8b cf                	mov    %edi,%ecx
  44ce2d:	e8 6e e4 ff ff       	call   0x44b2a0
  44ce32:	8b f0                	mov    %eax,%esi
  44ce34:	eb 03                	jmp    0x44ce39
  44ce36:	8b 73 10             	mov    0x10(%ebx),%esi
  44ce39:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44ce3d:	8d 43 08             	lea    0x8(%ebx),%eax
  44ce40:	74 07                	je     0x44ce49
  44ce42:	8b d7                	mov    %edi,%edx
  44ce44:	e8 b7 e5 ff ff       	call   0x44b400
  44ce49:	8b 4c 24 10          	mov    0x10(%esp),%ecx
  44ce4d:	03 f1                	add    %ecx,%esi
  44ce4f:	89 30                	mov    %esi,(%eax)
  44ce51:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44ce55:	03 cb                	add    %ebx,%ecx
  44ce57:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44ce5d:	e9 a7 e6 ff ff       	jmp    0x44b509
  44ce62:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44ce66:	d9 43 0c             	flds   0xc(%ebx)
  44ce69:	74 0b                	je     0x44ce76
  44ce6b:	51                   	push   %ecx
  44ce6c:	8b c7                	mov    %edi,%eax
  44ce6e:	d9 1c 24             	fstps  (%esp)
  44ce71:	e8 0a e2 ff ff       	call   0x44b080
  44ce76:	f6 43 06 04          	testb  $0x4,0x6(%ebx)
  44ce7a:	d9 5c 24 14          	fstps  0x14(%esp)
  44ce7e:	d9 43 10             	flds   0x10(%ebx)
  44ce81:	74 0b                	je     0x44ce8e
  44ce83:	51                   	push   %ecx
  44ce84:	8b c7                	mov    %edi,%eax
  44ce86:	d9 1c 24             	fstps  (%esp)
  44ce89:	e8 f2 e1 ff ff       	call   0x44b080
  44ce8e:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44ce92:	d9 5c 24 10          	fstps  0x10(%esp)
  44ce96:	8d 43 08             	lea    0x8(%ebx),%eax
  44ce99:	74 10                	je     0x44ceab
  44ce9b:	8b 75 08             	mov    0x8(%ebp),%esi
  44ce9e:	8b f8                	mov    %eax,%edi
  44cea0:	e8 db e4 ff ff       	call   0x44b380
  44cea5:	8b 5c 24 18          	mov    0x18(%esp),%ebx
  44cea9:	8b fe                	mov    %esi,%edi
  44ceab:	d9 44 24 10          	flds   0x10(%esp)
  44ceaf:	d8 44 24 14          	fadds  0x14(%esp)
  44ceb3:	d9 18                	fstps  (%eax)
  44ceb5:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44ceb9:	03 cb                	add    %ebx,%ecx
  44cebb:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44cec1:	e9 43 e6 ff ff       	jmp    0x44b509
  44cec6:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44ceca:	74 0e                	je     0x44ceda
  44cecc:	8b 43 0c             	mov    0xc(%ebx),%eax
  44cecf:	8b cf                	mov    %edi,%ecx
  44ced1:	e8 ca e3 ff ff       	call   0x44b2a0
  44ced6:	8b f0                	mov    %eax,%esi
  44ced8:	eb 03                	jmp    0x44cedd
  44ceda:	8b 73 0c             	mov    0xc(%ebx),%esi
  44cedd:	f6 43 06 04          	testb  $0x4,0x6(%ebx)
  44cee1:	74 10                	je     0x44cef3
  44cee3:	8b 43 10             	mov    0x10(%ebx),%eax
  44cee6:	8b cf                	mov    %edi,%ecx
  44cee8:	e8 b3 e3 ff ff       	call   0x44b2a0
  44ceed:	89 44 24 10          	mov    %eax,0x10(%esp)
  44cef1:	eb 07                	jmp    0x44cefa
  44cef3:	8b 53 10             	mov    0x10(%ebx),%edx
  44cef6:	89 54 24 10          	mov    %edx,0x10(%esp)
  44cefa:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44cefe:	8d 43 08             	lea    0x8(%ebx),%eax
  44cf01:	74 07                	je     0x44cf0a
  44cf03:	8b d7                	mov    %edi,%edx
  44cf05:	e8 f6 e4 ff ff       	call   0x44b400
  44cf0a:	2b 74 24 10          	sub    0x10(%esp),%esi
  44cf0e:	89 30                	mov    %esi,(%eax)
  44cf10:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44cf14:	03 cb                	add    %ebx,%ecx
  44cf16:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44cf1c:	e9 e8 e5 ff ff       	jmp    0x44b509
  44cf21:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44cf25:	d9 43 0c             	flds   0xc(%ebx)
  44cf28:	74 0b                	je     0x44cf35
  44cf2a:	51                   	push   %ecx
  44cf2b:	8b c7                	mov    %edi,%eax
  44cf2d:	d9 1c 24             	fstps  (%esp)
  44cf30:	e8 4b e1 ff ff       	call   0x44b080
  44cf35:	f6 43 06 04          	testb  $0x4,0x6(%ebx)
  44cf39:	d9 5c 24 10          	fstps  0x10(%esp)
  44cf3d:	d9 43 10             	flds   0x10(%ebx)
  44cf40:	74 0b                	je     0x44cf4d
  44cf42:	51                   	push   %ecx
  44cf43:	8b c7                	mov    %edi,%eax
  44cf45:	d9 1c 24             	fstps  (%esp)
  44cf48:	e8 33 e1 ff ff       	call   0x44b080
  44cf4d:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44cf51:	d9 5c 24 14          	fstps  0x14(%esp)
  44cf55:	8d 43 08             	lea    0x8(%ebx),%eax
  44cf58:	74 10                	je     0x44cf6a
  44cf5a:	8b 75 08             	mov    0x8(%ebp),%esi
  44cf5d:	8b f8                	mov    %eax,%edi
  44cf5f:	e8 1c e4 ff ff       	call   0x44b380
  44cf64:	8b 5c 24 18          	mov    0x18(%esp),%ebx
  44cf68:	8b fe                	mov    %esi,%edi
  44cf6a:	d9 44 24 10          	flds   0x10(%esp)
  44cf6e:	d8 64 24 14          	fsubs  0x14(%esp)
  44cf72:	d9 18                	fstps  (%eax)
  44cf74:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44cf78:	03 cb                	add    %ebx,%ecx
  44cf7a:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44cf80:	e9 84 e5 ff ff       	jmp    0x44b509
  44cf85:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44cf89:	8b 43 0c             	mov    0xc(%ebx),%eax
  44cf8c:	74 07                	je     0x44cf95
  44cf8e:	8b cf                	mov    %edi,%ecx
  44cf90:	e8 0b e3 ff ff       	call   0x44b2a0
  44cf95:	f6 43 06 04          	testb  $0x4,0x6(%ebx)
  44cf99:	89 44 24 10          	mov    %eax,0x10(%esp)
  44cf9d:	74 0e                	je     0x44cfad
  44cf9f:	8b 43 10             	mov    0x10(%ebx),%eax
  44cfa2:	8b cf                	mov    %edi,%ecx
  44cfa4:	e8 f7 e2 ff ff       	call   0x44b2a0
  44cfa9:	8b f0                	mov    %eax,%esi
  44cfab:	eb 03                	jmp    0x44cfb0
  44cfad:	8b 73 10             	mov    0x10(%ebx),%esi
  44cfb0:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44cfb4:	8d 43 08             	lea    0x8(%ebx),%eax
  44cfb7:	74 07                	je     0x44cfc0
  44cfb9:	8b d7                	mov    %edi,%edx
  44cfbb:	e8 40 e4 ff ff       	call   0x44b400
  44cfc0:	0f af 74 24 10       	imul   0x10(%esp),%esi
  44cfc5:	89 30                	mov    %esi,(%eax)
  44cfc7:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44cfcb:	03 cb                	add    %ebx,%ecx
  44cfcd:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44cfd3:	e9 31 e5 ff ff       	jmp    0x44b509
  44cfd8:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44cfdc:	d9 43 0c             	flds   0xc(%ebx)
  44cfdf:	74 0b                	je     0x44cfec
  44cfe1:	51                   	push   %ecx
  44cfe2:	8b c7                	mov    %edi,%eax
  44cfe4:	d9 1c 24             	fstps  (%esp)
  44cfe7:	e8 94 e0 ff ff       	call   0x44b080
  44cfec:	f6 43 06 04          	testb  $0x4,0x6(%ebx)
  44cff0:	d9 5c 24 14          	fstps  0x14(%esp)
  44cff4:	d9 43 10             	flds   0x10(%ebx)
  44cff7:	74 0b                	je     0x44d004
  44cff9:	51                   	push   %ecx
  44cffa:	8b c7                	mov    %edi,%eax
  44cffc:	d9 1c 24             	fstps  (%esp)
  44cfff:	e8 7c e0 ff ff       	call   0x44b080
  44d004:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44d008:	d9 5c 24 10          	fstps  0x10(%esp)
  44d00c:	8d 43 08             	lea    0x8(%ebx),%eax
  44d00f:	74 10                	je     0x44d021
  44d011:	8b 75 08             	mov    0x8(%ebp),%esi
  44d014:	8b f8                	mov    %eax,%edi
  44d016:	e8 65 e3 ff ff       	call   0x44b380
  44d01b:	8b 5c 24 18          	mov    0x18(%esp),%ebx
  44d01f:	8b fe                	mov    %esi,%edi
  44d021:	d9 44 24 10          	flds   0x10(%esp)
  44d025:	d8 4c 24 14          	fmuls  0x14(%esp)
  44d029:	d9 18                	fstps  (%eax)
  44d02b:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44d02f:	03 cb                	add    %ebx,%ecx
  44d031:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44d037:	e9 cd e4 ff ff       	jmp    0x44b509
  44d03c:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44d040:	74 10                	je     0x44d052
  44d042:	8b 43 0c             	mov    0xc(%ebx),%eax
  44d045:	8b cf                	mov    %edi,%ecx
  44d047:	e8 54 e2 ff ff       	call   0x44b2a0
  44d04c:	89 44 24 10          	mov    %eax,0x10(%esp)
  44d050:	eb 07                	jmp    0x44d059
  44d052:	8b 4b 0c             	mov    0xc(%ebx),%ecx
  44d055:	89 4c 24 10          	mov    %ecx,0x10(%esp)
  44d059:	f6 43 06 04          	testb  $0x4,0x6(%ebx)
  44d05d:	74 0e                	je     0x44d06d
  44d05f:	8b 43 10             	mov    0x10(%ebx),%eax
  44d062:	8b cf                	mov    %edi,%ecx
  44d064:	e8 37 e2 ff ff       	call   0x44b2a0
  44d069:	8b f0                	mov    %eax,%esi
  44d06b:	eb 03                	jmp    0x44d070
  44d06d:	8b 73 10             	mov    0x10(%ebx),%esi
  44d070:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44d074:	8d 43 08             	lea    0x8(%ebx),%eax
  44d077:	74 07                	je     0x44d080
  44d079:	8b d7                	mov    %edi,%edx
  44d07b:	e8 80 e3 ff ff       	call   0x44b400
  44d080:	8b c8                	mov    %eax,%ecx
  44d082:	8b 44 24 10          	mov    0x10(%esp),%eax
  44d086:	99                   	cltd
  44d087:	f7 fe                	idiv   %esi
  44d089:	89 01                	mov    %eax,(%ecx)
  44d08b:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44d08f:	03 cb                	add    %ebx,%ecx
  44d091:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44d097:	e9 6d e4 ff ff       	jmp    0x44b509
  44d09c:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44d0a0:	d9 43 0c             	flds   0xc(%ebx)
  44d0a3:	74 0b                	je     0x44d0b0
  44d0a5:	51                   	push   %ecx
  44d0a6:	8b c7                	mov    %edi,%eax
  44d0a8:	d9 1c 24             	fstps  (%esp)
  44d0ab:	e8 d0 df ff ff       	call   0x44b080
  44d0b0:	f6 43 06 04          	testb  $0x4,0x6(%ebx)
  44d0b4:	d9 5c 24 10          	fstps  0x10(%esp)
  44d0b8:	d9 43 10             	flds   0x10(%ebx)
  44d0bb:	74 0b                	je     0x44d0c8
  44d0bd:	51                   	push   %ecx
  44d0be:	8b c7                	mov    %edi,%eax
  44d0c0:	d9 1c 24             	fstps  (%esp)
  44d0c3:	e8 b8 df ff ff       	call   0x44b080
  44d0c8:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44d0cc:	d9 5c 24 14          	fstps  0x14(%esp)
  44d0d0:	8d 43 08             	lea    0x8(%ebx),%eax
  44d0d3:	74 10                	je     0x44d0e5
  44d0d5:	8b 75 08             	mov    0x8(%ebp),%esi
  44d0d8:	8b f8                	mov    %eax,%edi
  44d0da:	e8 a1 e2 ff ff       	call   0x44b380
  44d0df:	8b 5c 24 18          	mov    0x18(%esp),%ebx
  44d0e3:	8b fe                	mov    %esi,%edi
  44d0e5:	d9 44 24 10          	flds   0x10(%esp)
  44d0e9:	d8 74 24 14          	fdivs  0x14(%esp)
  44d0ed:	d9 18                	fstps  (%eax)
  44d0ef:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44d0f3:	03 cb                	add    %ebx,%ecx
  44d0f5:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44d0fb:	e9 09 e4 ff ff       	jmp    0x44b509
  44d100:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44d104:	74 10                	je     0x44d116
  44d106:	8b 43 0c             	mov    0xc(%ebx),%eax
  44d109:	8b cf                	mov    %edi,%ecx
  44d10b:	e8 90 e1 ff ff       	call   0x44b2a0
  44d110:	89 44 24 10          	mov    %eax,0x10(%esp)
  44d114:	eb 07                	jmp    0x44d11d
  44d116:	8b 53 0c             	mov    0xc(%ebx),%edx
  44d119:	89 54 24 10          	mov    %edx,0x10(%esp)
  44d11d:	f6 43 06 04          	testb  $0x4,0x6(%ebx)
  44d121:	74 0e                	je     0x44d131
  44d123:	8b 43 10             	mov    0x10(%ebx),%eax
  44d126:	8b cf                	mov    %edi,%ecx
  44d128:	e8 73 e1 ff ff       	call   0x44b2a0
  44d12d:	8b f0                	mov    %eax,%esi
  44d12f:	eb 03                	jmp    0x44d134
  44d131:	8b 73 10             	mov    0x10(%ebx),%esi
  44d134:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44d138:	8d 43 08             	lea    0x8(%ebx),%eax
  44d13b:	74 07                	je     0x44d144
  44d13d:	8b d7                	mov    %edi,%edx
  44d13f:	e8 bc e2 ff ff       	call   0x44b400
  44d144:	8b c8                	mov    %eax,%ecx
  44d146:	8b 44 24 10          	mov    0x10(%esp),%eax
  44d14a:	99                   	cltd
  44d14b:	f7 fe                	idiv   %esi
  44d14d:	89 11                	mov    %edx,(%ecx)
  44d14f:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44d153:	03 cb                	add    %ebx,%ecx
  44d155:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44d15b:	e9 a9 e3 ff ff       	jmp    0x44b509
  44d160:	f6 43 06 04          	testb  $0x4,0x6(%ebx)
  44d164:	d9 43 10             	flds   0x10(%ebx)
  44d167:	74 0b                	je     0x44d174
  44d169:	51                   	push   %ecx
  44d16a:	8b c7                	mov    %edi,%eax
  44d16c:	d9 1c 24             	fstps  (%esp)
  44d16f:	e8 0c df ff ff       	call   0x44b080
  44d174:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44d178:	d9 5c 24 14          	fstps  0x14(%esp)
  44d17c:	d9 43 0c             	flds   0xc(%ebx)
  44d17f:	74 0b                	je     0x44d18c
  44d181:	51                   	push   %ecx
  44d182:	8b c7                	mov    %edi,%eax
  44d184:	d9 1c 24             	fstps  (%esp)
  44d187:	e8 f4 de ff ff       	call   0x44b080
  44d18c:	d9 5c 24 10          	fstps  0x10(%esp)
  44d190:	d9 44 24 10          	flds   0x10(%esp)
  44d194:	d9 44 24 14          	flds   0x14(%esp)
  44d198:	e8 2d 95 03 00       	call   0x4866ca
  44d19d:	d9 5c 24 10          	fstps  0x10(%esp)
  44d1a1:	d9 44 24 10          	flds   0x10(%esp)
  44d1a5:	e9 2c 05 00 00       	jmp    0x44d6d6
  44d1aa:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44d1ae:	74 0e                	je     0x44d1be
  44d1b0:	8b 43 0c             	mov    0xc(%ebx),%eax
  44d1b3:	8b cf                	mov    %edi,%ecx
  44d1b5:	e8 e6 e0 ff ff       	call   0x44b2a0
  44d1ba:	8b f0                	mov    %eax,%esi
  44d1bc:	eb 03                	jmp    0x44d1c1
  44d1be:	8b 73 0c             	mov    0xc(%ebx),%esi
  44d1c1:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44d1c5:	8d 43 08             	lea    0x8(%ebx),%eax
  44d1c8:	74 07                	je     0x44d1d1
  44d1ca:	8b d7                	mov    %edi,%edx
  44d1cc:	e8 2f e2 ff ff       	call   0x44b400
  44d1d1:	01 30                	add    %esi,(%eax)
  44d1d3:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44d1d7:	03 cb                	add    %ebx,%ecx
  44d1d9:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44d1df:	e9 25 e3 ff ff       	jmp    0x44b509
  44d1e4:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44d1e8:	d9 43 0c             	flds   0xc(%ebx)
  44d1eb:	74 0b                	je     0x44d1f8
  44d1ed:	51                   	push   %ecx
  44d1ee:	8b c7                	mov    %edi,%eax
  44d1f0:	d9 1c 24             	fstps  (%esp)
  44d1f3:	e8 88 de ff ff       	call   0x44b080
  44d1f8:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44d1fc:	d9 5c 24 10          	fstps  0x10(%esp)
  44d200:	8d 43 08             	lea    0x8(%ebx),%eax
  44d203:	74 10                	je     0x44d215
  44d205:	8b 75 08             	mov    0x8(%ebp),%esi
  44d208:	8b f8                	mov    %eax,%edi
  44d20a:	e8 71 e1 ff ff       	call   0x44b380
  44d20f:	8b 5c 24 18          	mov    0x18(%esp),%ebx
  44d213:	8b fe                	mov    %esi,%edi
  44d215:	d9 44 24 10          	flds   0x10(%esp)
  44d219:	d8 00                	fadds  (%eax)
  44d21b:	d9 18                	fstps  (%eax)
  44d21d:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44d221:	03 cb                	add    %ebx,%ecx
  44d223:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44d229:	e9 db e2 ff ff       	jmp    0x44b509
  44d22e:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44d232:	74 0e                	je     0x44d242
  44d234:	8b 43 0c             	mov    0xc(%ebx),%eax
  44d237:	8b cf                	mov    %edi,%ecx
  44d239:	e8 62 e0 ff ff       	call   0x44b2a0
  44d23e:	8b f0                	mov    %eax,%esi
  44d240:	eb 03                	jmp    0x44d245
  44d242:	8b 73 0c             	mov    0xc(%ebx),%esi
  44d245:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44d249:	8d 43 08             	lea    0x8(%ebx),%eax
  44d24c:	74 07                	je     0x44d255
  44d24e:	8b d7                	mov    %edi,%edx
  44d250:	e8 ab e1 ff ff       	call   0x44b400
  44d255:	29 30                	sub    %esi,(%eax)
  44d257:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44d25b:	03 cb                	add    %ebx,%ecx
  44d25d:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44d263:	e9 a1 e2 ff ff       	jmp    0x44b509
  44d268:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44d26c:	d9 43 0c             	flds   0xc(%ebx)
  44d26f:	74 0b                	je     0x44d27c
  44d271:	51                   	push   %ecx
  44d272:	8b c7                	mov    %edi,%eax
  44d274:	d9 1c 24             	fstps  (%esp)
  44d277:	e8 04 de ff ff       	call   0x44b080
  44d27c:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44d280:	d9 5c 24 10          	fstps  0x10(%esp)
  44d284:	8d 43 08             	lea    0x8(%ebx),%eax
  44d287:	74 10                	je     0x44d299
  44d289:	8b 75 08             	mov    0x8(%ebp),%esi
  44d28c:	8b f8                	mov    %eax,%edi
  44d28e:	e8 ed e0 ff ff       	call   0x44b380
  44d293:	8b 5c 24 18          	mov    0x18(%esp),%ebx
  44d297:	8b fe                	mov    %esi,%edi
  44d299:	d9 00                	flds   (%eax)
  44d29b:	d8 64 24 10          	fsubs  0x10(%esp)
  44d29f:	d9 18                	fstps  (%eax)
  44d2a1:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44d2a5:	03 cb                	add    %ebx,%ecx
  44d2a7:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44d2ad:	e9 57 e2 ff ff       	jmp    0x44b509
  44d2b2:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44d2b6:	74 0e                	je     0x44d2c6
  44d2b8:	8b 43 0c             	mov    0xc(%ebx),%eax
  44d2bb:	8b cf                	mov    %edi,%ecx
  44d2bd:	e8 de df ff ff       	call   0x44b2a0
  44d2c2:	8b f0                	mov    %eax,%esi
  44d2c4:	eb 03                	jmp    0x44d2c9
  44d2c6:	8b 73 0c             	mov    0xc(%ebx),%esi
  44d2c9:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44d2cd:	8d 43 08             	lea    0x8(%ebx),%eax
  44d2d0:	74 07                	je     0x44d2d9
  44d2d2:	8b d7                	mov    %edi,%edx
  44d2d4:	e8 27 e1 ff ff       	call   0x44b400
  44d2d9:	8b 08                	mov    (%eax),%ecx
  44d2db:	0f af ce             	imul   %esi,%ecx
  44d2de:	89 08                	mov    %ecx,(%eax)
  44d2e0:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44d2e4:	03 cb                	add    %ebx,%ecx
  44d2e6:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44d2ec:	e9 18 e2 ff ff       	jmp    0x44b509
  44d2f1:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44d2f5:	d9 43 0c             	flds   0xc(%ebx)
  44d2f8:	74 0b                	je     0x44d305
  44d2fa:	51                   	push   %ecx
  44d2fb:	8b c7                	mov    %edi,%eax
  44d2fd:	d9 1c 24             	fstps  (%esp)
  44d300:	e8 7b dd ff ff       	call   0x44b080
  44d305:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44d309:	d9 5c 24 10          	fstps  0x10(%esp)
  44d30d:	8d 43 08             	lea    0x8(%ebx),%eax
  44d310:	74 10                	je     0x44d322
  44d312:	8b 75 08             	mov    0x8(%ebp),%esi
  44d315:	8b f8                	mov    %eax,%edi
  44d317:	e8 64 e0 ff ff       	call   0x44b380
  44d31c:	8b 5c 24 18          	mov    0x18(%esp),%ebx
  44d320:	8b fe                	mov    %esi,%edi
  44d322:	d9 00                	flds   (%eax)
  44d324:	d8 4c 24 10          	fmuls  0x10(%esp)
  44d328:	d9 18                	fstps  (%eax)
  44d32a:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44d32e:	03 cb                	add    %ebx,%ecx
  44d330:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44d336:	e9 ce e1 ff ff       	jmp    0x44b509
  44d33b:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44d33f:	74 0e                	je     0x44d34f
  44d341:	8b 43 0c             	mov    0xc(%ebx),%eax
  44d344:	8b cf                	mov    %edi,%ecx
  44d346:	e8 55 df ff ff       	call   0x44b2a0
  44d34b:	8b f0                	mov    %eax,%esi
  44d34d:	eb 03                	jmp    0x44d352
  44d34f:	8b 73 0c             	mov    0xc(%ebx),%esi
  44d352:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44d356:	8d 43 08             	lea    0x8(%ebx),%eax
  44d359:	74 07                	je     0x44d362
  44d35b:	8b d7                	mov    %edi,%edx
  44d35d:	e8 9e e0 ff ff       	call   0x44b400
  44d362:	8b c8                	mov    %eax,%ecx
  44d364:	8b 01                	mov    (%ecx),%eax
  44d366:	99                   	cltd
  44d367:	f7 fe                	idiv   %esi
  44d369:	89 01                	mov    %eax,(%ecx)
  44d36b:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44d36f:	03 cb                	add    %ebx,%ecx
  44d371:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44d377:	e9 8d e1 ff ff       	jmp    0x44b509
  44d37c:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44d380:	d9 43 0c             	flds   0xc(%ebx)
  44d383:	74 0b                	je     0x44d390
  44d385:	51                   	push   %ecx
  44d386:	8b c7                	mov    %edi,%eax
  44d388:	d9 1c 24             	fstps  (%esp)
  44d38b:	e8 f0 dc ff ff       	call   0x44b080
  44d390:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44d394:	d9 5c 24 10          	fstps  0x10(%esp)
  44d398:	8d 43 08             	lea    0x8(%ebx),%eax
  44d39b:	74 10                	je     0x44d3ad
  44d39d:	8b 75 08             	mov    0x8(%ebp),%esi
  44d3a0:	8b f8                	mov    %eax,%edi
  44d3a2:	e8 d9 df ff ff       	call   0x44b380
  44d3a7:	8b 5c 24 18          	mov    0x18(%esp),%ebx
  44d3ab:	8b fe                	mov    %esi,%edi
  44d3ad:	d9 00                	flds   (%eax)
  44d3af:	d8 74 24 10          	fdivs  0x10(%esp)
  44d3b3:	d9 18                	fstps  (%eax)
  44d3b5:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44d3b9:	03 cb                	add    %ebx,%ecx
  44d3bb:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44d3c1:	e9 43 e1 ff ff       	jmp    0x44b509
  44d3c6:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44d3ca:	74 0e                	je     0x44d3da
  44d3cc:	8b 43 0c             	mov    0xc(%ebx),%eax
  44d3cf:	8b cf                	mov    %edi,%ecx
  44d3d1:	e8 ca de ff ff       	call   0x44b2a0
  44d3d6:	8b f0                	mov    %eax,%esi
  44d3d8:	eb 03                	jmp    0x44d3dd
  44d3da:	8b 73 0c             	mov    0xc(%ebx),%esi
  44d3dd:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44d3e1:	8d 43 08             	lea    0x8(%ebx),%eax
  44d3e4:	74 07                	je     0x44d3ed
  44d3e6:	8b d7                	mov    %edi,%edx
  44d3e8:	e8 13 e0 ff ff       	call   0x44b400
  44d3ed:	8b c8                	mov    %eax,%ecx
  44d3ef:	8b 01                	mov    (%ecx),%eax
  44d3f1:	99                   	cltd
  44d3f2:	f7 fe                	idiv   %esi
  44d3f4:	89 11                	mov    %edx,(%ecx)
  44d3f6:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44d3fa:	03 cb                	add    %ebx,%ecx
  44d3fc:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44d402:	e9 02 e1 ff ff       	jmp    0x44b509
  44d407:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44d40b:	d9 43 0c             	flds   0xc(%ebx)
  44d40e:	74 0b                	je     0x44d41b
  44d410:	51                   	push   %ecx
  44d411:	8b c7                	mov    %edi,%eax
  44d413:	d9 1c 24             	fstps  (%esp)
  44d416:	e8 65 dc ff ff       	call   0x44b080
  44d41b:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44d41f:	d9 5c 24 14          	fstps  0x14(%esp)
  44d423:	d9 43 08             	flds   0x8(%ebx)
  44d426:	8d 73 08             	lea    0x8(%ebx),%esi
  44d429:	74 0b                	je     0x44d436
  44d42b:	51                   	push   %ecx
  44d42c:	8b c7                	mov    %edi,%eax
  44d42e:	d9 1c 24             	fstps  (%esp)
  44d431:	e8 4a dc ff ff       	call   0x44b080
  44d436:	d9 5c 24 10          	fstps  0x10(%esp)
  44d43a:	d9 44 24 10          	flds   0x10(%esp)
  44d43e:	d9 44 24 14          	flds   0x14(%esp)
  44d442:	e8 83 92 03 00       	call   0x4866ca
  44d447:	d9 5c 24 10          	fstps  0x10(%esp)
  44d44b:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44d44f:	d9 44 24 10          	flds   0x10(%esp)
  44d453:	d9 5c 24 10          	fstps  0x10(%esp)
  44d457:	75 19                	jne    0x44d472
  44d459:	d9 44 24 10          	flds   0x10(%esp)
  44d45d:	8b c6                	mov    %esi,%eax
  44d45f:	d9 18                	fstps  (%eax)
  44d461:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44d465:	03 cb                	add    %ebx,%ecx
  44d467:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44d46d:	e9 97 e0 ff ff       	jmp    0x44b509
  44d472:	8b fe                	mov    %esi,%edi
  44d474:	e9 6c 02 00 00       	jmp    0x44d6e5
  44d479:	f7 87 04 04 00 00 00 	testl  $0x40000000,0x404(%edi)
  44d480:	00 00 40 
  44d483:	8b 43 0c             	mov    0xc(%ebx),%eax
  44d486:	74 29                	je     0x44d4b1
  44d488:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44d48c:	74 07                	je     0x44d495
  44d48e:	8b cf                	mov    %edi,%ecx
  44d490:	e8 0b de ff ff       	call   0x44b2a0
  44d495:	89 44 24 10          	mov    %eax,0x10(%esp)
  44d499:	85 c0                	test   %eax,%eax
  44d49b:	74 3d                	je     0x44d4da
  44d49d:	be f8 2e 4c 00       	mov    $0x4c2ef8,%esi
  44d4a2:	e8 89 b7 00 00       	call   0x458c30
  44d4a7:	33 d2                	xor    %edx,%edx
  44d4a9:	f7 74 24 10          	divl   0x10(%esp)
  44d4ad:	8b f2                	mov    %edx,%esi
  44d4af:	eb 2b                	jmp    0x44d4dc
  44d4b1:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44d4b5:	74 07                	je     0x44d4be
  44d4b7:	8b cf                	mov    %edi,%ecx
  44d4b9:	e8 e2 dd ff ff       	call   0x44b2a0
  44d4be:	89 44 24 10          	mov    %eax,0x10(%esp)
  44d4c2:	85 c0                	test   %eax,%eax
  44d4c4:	74 14                	je     0x44d4da
  44d4c6:	be 00 2f 4c 00       	mov    $0x4c2f00,%esi
  44d4cb:	e8 60 b7 00 00       	call   0x458c30
  44d4d0:	33 d2                	xor    %edx,%edx
  44d4d2:	f7 74 24 10          	divl   0x10(%esp)
  44d4d6:	8b f2                	mov    %edx,%esi
  44d4d8:	eb 02                	jmp    0x44d4dc
  44d4da:	33 f6                	xor    %esi,%esi
  44d4dc:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44d4e0:	8d 43 08             	lea    0x8(%ebx),%eax
  44d4e3:	74 07                	je     0x44d4ec
  44d4e5:	8b d7                	mov    %edi,%edx
  44d4e7:	e8 14 df ff ff       	call   0x44b400
  44d4ec:	89 30                	mov    %esi,(%eax)
  44d4ee:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44d4f2:	03 cb                	add    %ebx,%ecx
  44d4f4:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44d4fa:	e9 0a e0 ff ff       	jmp    0x44b509
  44d4ff:	f7 87 04 04 00 00 00 	testl  $0x40000000,0x404(%edi)
  44d506:	00 00 40 
  44d509:	d9 43 0c             	flds   0xc(%ebx)
  44d50c:	74 20                	je     0x44d52e
  44d50e:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44d512:	74 0b                	je     0x44d51f
  44d514:	51                   	push   %ecx
  44d515:	8b c7                	mov    %edi,%eax
  44d517:	d9 1c 24             	fstps  (%esp)
  44d51a:	e8 61 db ff ff       	call   0x44b080
  44d51f:	d9 5c 24 10          	fstps  0x10(%esp)
  44d523:	be f8 2e 4c 00       	mov    $0x4c2ef8,%esi
  44d528:	d9 44 24 10          	flds   0x10(%esp)
  44d52c:	eb 1e                	jmp    0x44d54c
  44d52e:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44d532:	74 0b                	je     0x44d53f
  44d534:	51                   	push   %ecx
  44d535:	8b c7                	mov    %edi,%eax
  44d537:	d9 1c 24             	fstps  (%esp)
  44d53a:	e8 41 db ff ff       	call   0x44b080
  44d53f:	d9 5c 24 10          	fstps  0x10(%esp)
  44d543:	be 00 2f 4c 00       	mov    $0x4c2f00,%esi
  44d548:	d9 44 24 10          	flds   0x10(%esp)
  44d54c:	51                   	push   %ecx
  44d54d:	d9 1c 24             	fstps  (%esp)
  44d550:	e8 9b e4 fb ff       	call   0x40b9f0
  44d555:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44d559:	d9 5c 24 10          	fstps  0x10(%esp)
  44d55d:	8d 43 08             	lea    0x8(%ebx),%eax
  44d560:	74 10                	je     0x44d572
  44d562:	8b 75 08             	mov    0x8(%ebp),%esi
  44d565:	8b f8                	mov    %eax,%edi
  44d567:	e8 14 de ff ff       	call   0x44b380
  44d56c:	8b 5c 24 18          	mov    0x18(%esp),%ebx
  44d570:	8b fe                	mov    %esi,%edi
  44d572:	d9 44 24 10          	flds   0x10(%esp)
  44d576:	d9 18                	fstps  (%eax)
  44d578:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44d57c:	03 cb                	add    %ebx,%ecx
  44d57e:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44d584:	e9 80 df ff ff       	jmp    0x44b509
  44d589:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44d58d:	d9 43 0c             	flds   0xc(%ebx)
  44d590:	74 0b                	je     0x44d59d
  44d592:	51                   	push   %ecx
  44d593:	8b c7                	mov    %edi,%eax
  44d595:	d9 1c 24             	fstps  (%esp)
  44d598:	e8 e3 da ff ff       	call   0x44b080
  44d59d:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44d5a1:	d9 5c 24 10          	fstps  0x10(%esp)
  44d5a5:	8d 43 08             	lea    0x8(%ebx),%eax
  44d5a8:	74 10                	je     0x44d5ba
  44d5aa:	8b 75 08             	mov    0x8(%ebp),%esi
  44d5ad:	8b f8                	mov    %eax,%edi
  44d5af:	e8 cc dd ff ff       	call   0x44b380
  44d5b4:	8b 5c 24 18          	mov    0x18(%esp),%ebx
  44d5b8:	8b fe                	mov    %esi,%edi
  44d5ba:	d9 44 24 10          	flds   0x10(%esp)
  44d5be:	51                   	push   %ecx
  44d5bf:	d9 1c 24             	fstps  (%esp)
  44d5c2:	8b f0                	mov    %eax,%esi
  44d5c4:	e8 e7 89 fb ff       	call   0x405fb0
  44d5c9:	d9 1e                	fstps  (%esi)
  44d5cb:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44d5cf:	03 cb                	add    %ebx,%ecx
  44d5d1:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44d5d7:	e9 2d df ff ff       	jmp    0x44b509
  44d5dc:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44d5e0:	d9 43 0c             	flds   0xc(%ebx)
  44d5e3:	74 0b                	je     0x44d5f0
  44d5e5:	51                   	push   %ecx
  44d5e6:	8b c7                	mov    %edi,%eax
  44d5e8:	d9 1c 24             	fstps  (%esp)
  44d5eb:	e8 90 da ff ff       	call   0x44b080
  44d5f0:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44d5f4:	d9 5c 24 10          	fstps  0x10(%esp)
  44d5f8:	8d 43 08             	lea    0x8(%ebx),%eax
  44d5fb:	74 10                	je     0x44d60d
  44d5fd:	8b 75 08             	mov    0x8(%ebp),%esi
  44d600:	8b f8                	mov    %eax,%edi
  44d602:	e8 79 dd ff ff       	call   0x44b380
  44d607:	8b 5c 24 18          	mov    0x18(%esp),%ebx
  44d60b:	8b fe                	mov    %esi,%edi
  44d60d:	d9 44 24 10          	flds   0x10(%esp)
  44d611:	51                   	push   %ecx
  44d612:	d9 1c 24             	fstps  (%esp)
  44d615:	8b f0                	mov    %eax,%esi
  44d617:	e8 44 c0 fc ff       	call   0x419660
  44d61c:	d9 1e                	fstps  (%esi)
  44d61e:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44d622:	03 cb                	add    %ebx,%ecx
  44d624:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44d62a:	e9 da de ff ff       	jmp    0x44b509
  44d62f:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44d633:	d9 43 0c             	flds   0xc(%ebx)
  44d636:	74 0b                	je     0x44d643
  44d638:	51                   	push   %ecx
  44d639:	8b c7                	mov    %edi,%eax
  44d63b:	d9 1c 24             	fstps  (%esp)
  44d63e:	e8 3d da ff ff       	call   0x44b080
  44d643:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44d647:	d9 5c 24 10          	fstps  0x10(%esp)
  44d64b:	8d 43 08             	lea    0x8(%ebx),%eax
  44d64e:	74 10                	je     0x44d660
  44d650:	8b 75 08             	mov    0x8(%ebp),%esi
  44d653:	8b f8                	mov    %eax,%edi
  44d655:	e8 26 dd ff ff       	call   0x44b380
  44d65a:	8b 5c 24 18          	mov    0x18(%esp),%ebx
  44d65e:	8b fe                	mov    %esi,%edi
  44d660:	d9 44 24 10          	flds   0x10(%esp)
  44d664:	51                   	push   %ecx
  44d665:	d9 1c 24             	fstps  (%esp)
  44d668:	8b f0                	mov    %eax,%esi
  44d66a:	e8 61 e0 fd ff       	call   0x42b6d0
  44d66f:	d9 1e                	fstps  (%esi)
  44d671:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44d675:	03 cb                	add    %ebx,%ecx
  44d677:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44d67d:	e9 87 de ff ff       	jmp    0x44b509
  44d682:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44d686:	d9 43 0c             	flds   0xc(%ebx)
  44d689:	74 0b                	je     0x44d696
  44d68b:	51                   	push   %ecx
  44d68c:	8b c7                	mov    %edi,%eax
  44d68e:	d9 1c 24             	fstps  (%esp)
  44d691:	e8 ea d9 ff ff       	call   0x44b080
  44d696:	d9 5c 24 10          	fstps  0x10(%esp)
  44d69a:	d9 44 24 10          	flds   0x10(%esp)
  44d69e:	e8 9d 90 03 00       	call   0x486740
  44d6a3:	d9 5c 24 10          	fstps  0x10(%esp)
  44d6a7:	d9 44 24 10          	flds   0x10(%esp)
  44d6ab:	eb 29                	jmp    0x44d6d6
  44d6ad:	f6 43 06 02          	testb  $0x2,0x6(%ebx)
  44d6b1:	d9 43 0c             	flds   0xc(%ebx)
  44d6b4:	74 0b                	je     0x44d6c1
  44d6b6:	51                   	push   %ecx
  44d6b7:	8b c7                	mov    %edi,%eax
  44d6b9:	d9 1c 24             	fstps  (%esp)
  44d6bc:	e8 bf d9 ff ff       	call   0x44b080
  44d6c1:	d9 5c 24 10          	fstps  0x10(%esp)
  44d6c5:	d9 44 24 10          	flds   0x10(%esp)
  44d6c9:	e8 c2 91 03 00       	call   0x486890
  44d6ce:	d9 5c 24 10          	fstps  0x10(%esp)
  44d6d2:	d9 44 24 10          	flds   0x10(%esp)
  44d6d6:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44d6da:	d9 5c 24 10          	fstps  0x10(%esp)
  44d6de:	8d 43 08             	lea    0x8(%ebx),%eax
  44d6e1:	74 10                	je     0x44d6f3
  44d6e3:	8b f8                	mov    %eax,%edi
  44d6e5:	8b 75 08             	mov    0x8(%ebp),%esi
  44d6e8:	e8 93 dc ff ff       	call   0x44b380
  44d6ed:	8b 5c 24 18          	mov    0x18(%esp),%ebx
  44d6f1:	8b fe                	mov    %esi,%edi
  44d6f3:	d9 44 24 10          	flds   0x10(%esp)
  44d6f7:	d9 18                	fstps  (%eax)
  44d6f9:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44d6fd:	03 cb                	add    %ebx,%ecx
  44d6ff:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44d705:	e9 ff dd ff ff       	jmp    0x44b509
  44d70a:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44d70e:	d9 43 08             	flds   0x8(%ebx)
  44d711:	8d 73 08             	lea    0x8(%ebx),%esi
  44d714:	74 0b                	je     0x44d721
  44d716:	51                   	push   %ecx
  44d717:	8b c7                	mov    %edi,%eax
  44d719:	d9 1c 24             	fstps  (%esp)
  44d71c:	e8 5f d9 ff ff       	call   0x44b080
  44d721:	f6 43 06 01          	testb  $0x1,0x6(%ebx)
  44d725:	d9 5c 24 10          	fstps  0x10(%esp)
  44d729:	74 13                	je     0x44d73e
  44d72b:	8b fe                	mov    %esi,%edi
  44d72d:	8b 75 08             	mov    0x8(%ebp),%esi
  44d730:	e8 4b dc ff ff       	call   0x44b380
  44d735:	8b 5c 24 18          	mov    0x18(%esp),%ebx
  44d739:	8b 7d 08             	mov    0x8(%ebp),%edi
  44d73c:	8b f0                	mov    %eax,%esi
  44d73e:	d9 ee                	fldz
  44d740:	83 ec 08             	sub    $0x8,%esp
  44d743:	d9 5c 24 04          	fstps  0x4(%esp)
  44d747:	d9 44 24 18          	flds   0x18(%esp)
  44d74b:	d9 1c 24             	fstps  (%esp)
  44d74e:	e8 dd b6 00 00       	call   0x458e30
  44d753:	d9 1e                	fstps  (%esi)
  44d755:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44d759:	03 cb                	add    %ebx,%ecx
  44d75b:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44d761:	e9 a3 dd ff ff       	jmp    0x44b509
  44d766:	0f b6 53 08          	movzbl 0x8(%ebx),%edx
  44d76a:	c1 e2 1c             	shl    $0x1c,%edx
  44d76d:	33 97 04 04 00 00    	xor    0x404(%edi),%edx
  44d773:	81 e2 00 00 00 10    	and    $0x10000000,%edx
  44d779:	31 97 04 04 00 00    	xor    %edx,0x404(%edi)
  44d77f:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44d783:	03 cb                	add    %ebx,%ecx
  44d785:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44d78b:	e9 79 dd ff ff       	jmp    0x44b509
  44d790:	0f b6 43 08          	movzbl 0x8(%ebx),%eax
  44d794:	89 47 20             	mov    %eax,0x20(%edi)
  44d797:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44d79b:	03 cb                	add    %ebx,%ecx
  44d79d:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44d7a3:	e9 61 dd ff ff       	jmp    0x44b509
  44d7a8:	0f b6 4b 08          	movzbl 0x8(%ebx),%ecx
  44d7ac:	c1 e1 0f             	shl    $0xf,%ecx
  44d7af:	33 8f 04 04 00 00    	xor    0x404(%edi),%ecx
  44d7b5:	81 e1 00 80 00 00    	and    $0x8000,%ecx
  44d7bb:	31 8f 04 04 00 00    	xor    %ecx,0x404(%edi)
  44d7c1:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44d7c5:	03 cb                	add    %ebx,%ecx
  44d7c7:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44d7cd:	e9 37 dd ff ff       	jmp    0x44b509
  44d7d2:	0f b6 53 08          	movzbl 0x8(%ebx),%edx
  44d7d6:	c1 e2 1b             	shl    $0x1b,%edx
  44d7d9:	33 97 04 04 00 00    	xor    0x404(%edi),%edx
  44d7df:	81 e2 00 00 00 08    	and    $0x8000000,%edx
  44d7e5:	31 97 04 04 00 00    	xor    %edx,0x404(%edi)
  44d7eb:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44d7ef:	03 cb                	add    %ebx,%ecx
  44d7f1:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44d7f7:	e9 0d dd ff ff       	jmp    0x44b509
  44d7fc:	0f b6 43 08          	movzbl 0x8(%ebx),%eax
  44d800:	c1 e0 1e             	shl    $0x1e,%eax
  44d803:	33 87 04 04 00 00    	xor    0x404(%edi),%eax
  44d809:	25 00 00 00 40       	and    $0x40000000,%eax
  44d80e:	31 87 04 04 00 00    	xor    %eax,0x404(%edi)
  44d814:	0f b7 4b 02          	movzwl 0x2(%ebx),%ecx
  44d818:	03 cb                	add    %ebx,%ecx
  44d81a:	89 8f a8 03 00 00    	mov    %ecx,0x3a8(%edi)
  44d820:	e9 e4 dc ff ff       	jmp    0x44b509
  44d825:	bb 04 00 00 00       	mov    $0x4,%ebx
  44d82a:	d9 ee                	fldz
  44d82c:	d8 5f 34             	fcomps 0x34(%edi)
  44d82f:	df e0                	fnstsw %ax
  44d831:	f6 c4 44             	test   $0x44,%ah
  44d834:	7b 2c                	jnp    0x44d862
  44d836:	d9 47 34             	flds   0x34(%edi)
  44d839:	83 ec 08             	sub    $0x8,%esp
  44d83c:	d8 0d 48 79 4a 00    	fmuls  0x4a7948
  44d842:	d9 5c 24 24          	fstps  0x24(%esp)
  44d846:	d9 44 24 24          	flds   0x24(%esp)
  44d84a:	d9 5c 24 04          	fstps  0x4(%esp)
  44d84e:	d9 47 28             	flds   0x28(%edi)
  44d851:	d9 1c 24             	fstps  (%esp)
  44d854:	e8 d7 b5 00 00       	call   0x458e30
  44d859:	09 9f 04 04 00 00    	or     %ebx,0x404(%edi)
  44d85f:	d9 5f 28             	fstps  0x28(%edi)
  44d862:	d9 ee                	fldz
  44d864:	d8 5f 38             	fcomps 0x38(%edi)
  44d867:	df e0                	fnstsw %ax
  44d869:	f6 c4 44             	test   $0x44,%ah
  44d86c:	7b 2c                	jnp    0x44d89a
  44d86e:	d9 05 48 79 4a 00    	flds   0x4a7948
  44d874:	83 ec 08             	sub    $0x8,%esp
  44d877:	d8 4f 38             	fmuls  0x38(%edi)
  44d87a:	d9 5c 24 24          	fstps  0x24(%esp)
  44d87e:	d9 44 24 24          	flds   0x24(%esp)
  44d882:	d9 5c 24 04          	fstps  0x4(%esp)
  44d886:	d9 47 2c             	flds   0x2c(%edi)
  44d889:	d9 1c 24             	fstps  (%esp)
  44d88c:	e8 9f b5 00 00       	call   0x458e30
  44d891:	09 9f 04 04 00 00    	or     %ebx,0x404(%edi)
  44d897:	d9 5f 2c             	fstps  0x2c(%edi)
  44d89a:	d9 ee                	fldz
  44d89c:	d8 57 48             	fcoms  0x48(%edi)
  44d89f:	df e0                	fnstsw %ax
  44d8a1:	f6 c4 44             	test   $0x44,%ah
  44d8a4:	7b 16                	jnp    0x44d8bc
  44d8a6:	d9 05 48 79 4a 00    	flds   0x4a7948
  44d8ac:	83 8f 04 04 00 00 08 	orl    $0x8,0x404(%edi)
  44d8b3:	d8 4f 48             	fmuls  0x48(%edi)
  44d8b6:	d8 47 40             	fadds  0x40(%edi)
  44d8b9:	d9 5f 40             	fstps  0x40(%edi)
  44d8bc:	d8 57 44             	fcoms  0x44(%edi)
  44d8bf:	df e0                	fnstsw %ax
  44d8c1:	f6 c4 44             	test   $0x44,%ah
  44d8c4:	7b 16                	jnp    0x44d8dc
  44d8c6:	d9 47 44             	flds   0x44(%edi)
  44d8c9:	d8 0d 48 79 4a 00    	fmuls  0x4a7948
  44d8cf:	83 8f 04 04 00 00 0c 	orl    $0xc,0x404(%edi)
  44d8d6:	d8 47 3c             	fadds  0x3c(%edi)
  44d8d9:	d9 5f 3c             	fstps  0x3c(%edi)
  44d8dc:	d9 05 48 79 4a 00    	flds   0x4a7948
  44d8e2:	d8 8f ac 02 00 00    	fmuls  0x2ac(%edi)
  44d8e8:	d8 47 54             	fadds  0x54(%edi)
  44d8eb:	d9 5c 24 1c          	fstps  0x1c(%esp)
  44d8ef:	d9 44 24 1c          	flds   0x1c(%esp)
  44d8f3:	d9 57 54             	fsts   0x54(%edi)
  44d8f6:	d9 e8                	fld1
  44d8f8:	d8 d1                	fcom   %st(1)
  44d8fa:	df e0                	fnstsw %ax
  44d8fc:	d9 e8                	fld1
  44d8fe:	f6 c4 41             	test   $0x41,%ah
  44d901:	7a 09                	jp     0x44d90c
  44d903:	dc ea                	fsubr  %st,%st(2)
  44d905:	d9 ca                	fxch   %st(2)
  44d907:	d9 5f 54             	fstps  0x54(%edi)
  44d90a:	eb 14                	jmp    0x44d920
  44d90c:	d9 ca                	fxch   %st(2)
  44d90e:	d8 d3                	fcom   %st(3)
  44d910:	df e0                	fnstsw %ax
  44d912:	f6 c4 05             	test   $0x5,%ah
  44d915:	7a 07                	jp     0x44d91e
  44d917:	d8 c2                	fadd   %st(2),%st
  44d919:	d9 5f 54             	fstps  0x54(%edi)
  44d91c:	eb 02                	jmp    0x44d920
  44d91e:	dd d8                	fstp   %st(0)
  44d920:	d9 87 b0 02 00 00    	flds   0x2b0(%edi)
  44d926:	d8 0d 48 79 4a 00    	fmuls  0x4a7948
  44d92c:	d8 47 58             	fadds  0x58(%edi)
  44d92f:	d9 5c 24 1c          	fstps  0x1c(%esp)
  44d933:	d9 44 24 1c          	flds   0x1c(%esp)
  44d937:	d9 57 58             	fsts   0x58(%edi)
  44d93a:	d8 d1                	fcom   %st(1)
  44d93c:	df e0                	fnstsw %ax
  44d93e:	dd d9                	fstp   %st(1)
  44d940:	f6 c4 01             	test   $0x1,%ah
  44d943:	75 09                	jne    0x44d94e
  44d945:	dd da                	fstp   %st(2)
  44d947:	de e9                	fsubrp %st,%st(1)
  44d949:	d9 5f 58             	fstps  0x58(%edi)
  44d94c:	eb 16                	jmp    0x44d964
  44d94e:	d8 d2                	fcom   %st(2)
  44d950:	df e0                	fnstsw %ax
  44d952:	dd da                	fstp   %st(2)
  44d954:	f6 c4 05             	test   $0x5,%ah
  44d957:	7a 07                	jp     0x44d960
  44d959:	de c1                	faddp  %st,%st(1)
  44d95b:	d9 5f 58             	fstps  0x58(%edi)
  44d95e:	eb 04                	jmp    0x44d964
  44d960:	dd d9                	fstp   %st(1)
  44d962:	dd d8                	fstp   %st(0)
  44d964:	f7 87 04 04 00 00 00 	testl  $0x2000,0x404(%edi)
  44d96b:	20 00 00 
  44d96e:	74 36                	je     0x44d9a6
  44d970:	d9 87 e8 03 00 00    	flds   0x3e8(%edi)
  44d976:	d8 05 a4 37 4c 00    	fadds  0x4c37a4
  44d97c:	d9 9f e8 03 00 00    	fstps  0x3e8(%edi)
  44d982:	d9 05 a8 37 4c 00    	flds   0x4c37a8
  44d988:	d8 87 ec 03 00 00    	fadds  0x3ec(%edi)
  44d98e:	d9 9f ec 03 00 00    	fstps  0x3ec(%edi)
  44d994:	d9 87 f0 03 00 00    	flds   0x3f0(%edi)
  44d99a:	d8 05 ac 37 4c 00    	fadds  0x4c37ac
  44d9a0:	d9 9f f0 03 00 00    	fstps  0x3f0(%edi)
  44d9a6:	84 9f 08 04 00 00    	test   %bl,0x408(%edi)
  44d9ac:	0f 84 62 01 00 00    	je     0x44db14
  44d9b2:	8d 8c 24 a8 00 00 00 	lea    0xa8(%esp),%ecx
  44d9b9:	8b c7                	mov    %edi,%eax
  44d9bb:	e8 50 46 00 00       	call   0x452010
  44d9c0:	d9 84 24 a8 00 00 00 	flds   0xa8(%esp)
  44d9c7:	dd 05 90 7e 49 00    	fldl   0x497e90
  44d9cd:	dc f9                	fdivr  %st,%st(1)
  44d9cf:	d9 c9                	fxch   %st(1)
  44d9d1:	d9 5c 24 1c          	fstps  0x1c(%esp)
  44d9d5:	d9 44 24 1c          	flds   0x1c(%esp)
  44d9d9:	d9 57 70             	fsts   0x70(%edi)
  44d9dc:	d9 84 24 ac 00 00 00 	flds   0xac(%esp)
  44d9e3:	dd 05 88 7e 49 00    	fldl   0x497e88
  44d9e9:	dc f9                	fdivr  %st,%st(1)
  44d9eb:	d9 c9                	fxch   %st(1)
  44d9ed:	d9 5c 24 1c          	fstps  0x1c(%esp)
  44d9f1:	d9 44 24 1c          	flds   0x1c(%esp)
  44d9f5:	d9 57 74             	fsts   0x74(%edi)
  44d9f8:	d9 ee                	fldz
  44d9fa:	d8 d3                	fcom   %st(3)
  44d9fc:	df e0                	fnstsw %ax
  44d9fe:	dd db                	fstp   %st(3)
  44da00:	f6 c4 41             	test   $0x41,%ah
  44da03:	75 07                	jne    0x44da0c
  44da05:	d9 ca                	fxch   %st(2)
  44da07:	d9 57 70             	fsts   0x70(%edi)
  44da0a:	d9 ca                	fxch   %st(2)
  44da0c:	d8 da                	fcomp  %st(2)
  44da0e:	df e0                	fnstsw %ax
  44da10:	f6 c4 05             	test   $0x5,%ah
  44da13:	7a 07                	jp     0x44da1c
  44da15:	d9 c9                	fxch   %st(1)
  44da17:	d9 57 74             	fsts   0x74(%edi)
  44da1a:	d9 c9                	fxch   %st(1)
  44da1c:	d9 84 24 b4 00 00 00 	flds   0xb4(%esp)
  44da23:	d8 f3                	fdiv   %st(3),%st
  44da25:	d9 5c 24 1c          	fstps  0x1c(%esp)
  44da29:	d9 44 24 1c          	flds   0x1c(%esp)
  44da2d:	d9 57 78             	fsts   0x78(%edi)
  44da30:	d9 84 24 b8 00 00 00 	flds   0xb8(%esp)
  44da37:	d8 f2                	fdiv   %st(2),%st
  44da39:	d9 5c 24 1c          	fstps  0x1c(%esp)
  44da3d:	d9 44 24 1c          	flds   0x1c(%esp)
  44da41:	d9 57 7c             	fsts   0x7c(%edi)
  44da44:	d9 c9                	fxch   %st(1)
  44da46:	d8 db                	fcomp  %st(3)
  44da48:	df e0                	fnstsw %ax
  44da4a:	f6 c4 05             	test   $0x5,%ah
  44da4d:	7a 07                	jp     0x44da56
  44da4f:	d9 ca                	fxch   %st(2)
  44da51:	d9 57 78             	fsts   0x78(%edi)
  44da54:	d9 ca                	fxch   %st(2)
  44da56:	d8 da                	fcomp  %st(2)
  44da58:	df e0                	fnstsw %ax
  44da5a:	f6 c4 05             	test   $0x5,%ah
  44da5d:	7a 07                	jp     0x44da66
  44da5f:	d9 c9                	fxch   %st(1)
  44da61:	d9 57 7c             	fsts   0x7c(%edi)
  44da64:	d9 c9                	fxch   %st(1)
  44da66:	d9 84 24 c0 00 00 00 	flds   0xc0(%esp)
  44da6d:	d8 f3                	fdiv   %st(3),%st
  44da6f:	d9 5c 24 1c          	fstps  0x1c(%esp)
  44da73:	d9 44 24 1c          	flds   0x1c(%esp)
  44da77:	d9 97 80 00 00 00    	fsts   0x80(%edi)
  44da7d:	d9 84 24 c4 00 00 00 	flds   0xc4(%esp)
  44da84:	d8 f2                	fdiv   %st(2),%st
  44da86:	d9 5c 24 1c          	fstps  0x1c(%esp)
  44da8a:	d9 44 24 1c          	flds   0x1c(%esp)
  44da8e:	d9 97 84 00 00 00    	fsts   0x84(%edi)
  44da94:	d9 c9                	fxch   %st(1)
  44da96:	d8 db                	fcomp  %st(3)
  44da98:	df e0                	fnstsw %ax
  44da9a:	f6 c4 05             	test   $0x5,%ah
  44da9d:	7a 0a                	jp     0x44daa9
  44da9f:	d9 ca                	fxch   %st(2)
  44daa1:	d9 97 80 00 00 00    	fsts   0x80(%edi)
  44daa7:	d9 ca                	fxch   %st(2)
  44daa9:	d8 da                	fcomp  %st(2)
  44daab:	df e0                	fnstsw %ax
  44daad:	f6 c4 05             	test   $0x5,%ah
  44dab0:	7a 0a                	jp     0x44dabc
  44dab2:	d9 c9                	fxch   %st(1)
  44dab4:	d9 97 84 00 00 00    	fsts   0x84(%edi)
  44daba:	d9 c9                	fxch   %st(1)
  44dabc:	d9 84 24 cc 00 00 00 	flds   0xcc(%esp)
  44dac3:	de f3                	fdivp  %st,%st(3)
  44dac5:	d9 ca                	fxch   %st(2)
  44dac7:	d9 5c 24 1c          	fstps  0x1c(%esp)
  44dacb:	d9 44 24 1c          	flds   0x1c(%esp)
  44dacf:	d9 97 88 00 00 00    	fsts   0x88(%edi)
  44dad5:	d9 84 24 d0 00 00 00 	flds   0xd0(%esp)
  44dadc:	de f3                	fdivp  %st,%st(3)
  44dade:	d9 ca                	fxch   %st(2)
  44dae0:	d9 5c 24 1c          	fstps  0x1c(%esp)
  44dae4:	d9 44 24 1c          	flds   0x1c(%esp)
  44dae8:	d9 97 8c 00 00 00    	fsts   0x8c(%edi)
  44daee:	d9 ca                	fxch   %st(2)
  44daf0:	d8 d9                	fcomp  %st(1)
  44daf2:	df e0                	fnstsw %ax
  44daf4:	f6 c4 05             	test   $0x5,%ah
  44daf7:	7a 06                	jp     0x44daff
  44daf9:	d9 97 88 00 00 00    	fsts   0x88(%edi)
  44daff:	d8 d1                	fcom   %st(1)
  44db01:	df e0                	fnstsw %ax
  44db03:	dd d9                	fstp   %st(1)
  44db05:	f6 c4 41             	test   $0x41,%ah
  44db08:	75 08                	jne    0x44db12
  44db0a:	d9 9f 8c 00 00 00    	fstps  0x8c(%edi)
  44db10:	eb 02                	jmp    0x44db14
  44db12:	dd d8                	fstp   %st(0)
  44db14:	83 bf d4 00 00 00 00 	cmpl   $0x0,0xd4(%edi)
  44db1b:	74 64                	je     0x44db81
  44db1d:	f7 87 04 04 00 00 00 	testl  $0x100,0x404(%edi)
  44db24:	01 00 00 
  44db27:	8d 5c 24 24          	lea    0x24(%esp),%ebx
  44db2b:	75 2a                	jne    0x44db57
  44db2d:	81 c7 90 00 00 00    	add    $0x90,%edi
  44db33:	e8 08 77 fb ff       	call   0x405240
  44db38:	8b 10                	mov    (%eax),%edx
  44db3a:	8b 4d 08             	mov    0x8(%ebp),%ecx
  44db3d:	89 91 dc 03 00 00    	mov    %edx,0x3dc(%ecx)
  44db43:	8b 50 04             	mov    0x4(%eax),%edx
  44db46:	89 91 e0 03 00 00    	mov    %edx,0x3e0(%ecx)
  44db4c:	8b 40 08             	mov    0x8(%eax),%eax
  44db4f:	89 81 e4 03 00 00    	mov    %eax,0x3e4(%ecx)
  44db55:	eb 28                	jmp    0x44db7f
  44db57:	81 c7 90 00 00 00    	add    $0x90,%edi
  44db5d:	e8 de 76 fb ff       	call   0x405240
  44db62:	8b 10                	mov    (%eax),%edx
  44db64:	8b 4d 08             	mov    0x8(%ebp),%ecx
  44db67:	89 91 f4 03 00 00    	mov    %edx,0x3f4(%ecx)
  44db6d:	8b 50 04             	mov    0x4(%eax),%edx
  44db70:	89 91 f8 03 00 00    	mov    %edx,0x3f8(%ecx)
  44db76:	8b 40 08             	mov    0x8(%eax),%eax
  44db79:	89 81 fc 03 00 00    	mov    %eax,0x3fc(%ecx)
  44db7f:	8b f9                	mov    %ecx,%edi
  44db81:	83 bf 20 01 00 00 00 	cmpl   $0x0,0x120(%edi)
  44db88:	74 2d                	je     0x44dbb7
  44db8a:	8d 87 dc 00 00 00    	lea    0xdc(%edi),%eax
  44db90:	8d 5c 24 24          	lea    0x24(%esp),%ebx
  44db94:	e8 17 09 00 00       	call   0x44e4b0
  44db99:	8a 4c 24 2c          	mov    0x2c(%esp),%cl
  44db9d:	8a 54 24 28          	mov    0x28(%esp),%dl
  44dba1:	8a 44 24 24          	mov    0x24(%esp),%al
  44dba5:	88 8f 76 03 00 00    	mov    %cl,0x376(%edi)
  44dbab:	88 97 75 03 00 00    	mov    %dl,0x375(%edi)
  44dbb1:	88 87 74 03 00 00    	mov    %al,0x374(%edi)
  44dbb7:	83 bf 4c 01 00 00 00 	cmpl   $0x0,0x14c(%edi)
  44dbbe:	74 16                	je     0x44dbd6
  44dbc0:	81 c7 28 01 00 00    	add    $0x128,%edi
  44dbc6:	e8 f5 0b 00 00       	call   0x44e7c0
  44dbcb:	8b 4d 08             	mov    0x8(%ebp),%ecx
  44dbce:	88 81 77 03 00 00    	mov    %al,0x377(%ecx)
  44dbd4:	8b f9                	mov    %ecx,%edi
  44dbd6:	83 bf d4 01 00 00 00 	cmpl   $0x0,0x1d4(%edi)
  44dbdd:	74 29                	je     0x44dc08
  44dbdf:	81 c7 a0 01 00 00    	add    $0x1a0,%edi
  44dbe5:	8d 9c 24 88 00 00 00 	lea    0x88(%esp),%ebx
  44dbec:	e8 1f 0d 00 00       	call   0x44e910
  44dbf1:	8b 10                	mov    (%eax),%edx
  44dbf3:	8b 4d 08             	mov    0x8(%ebp),%ecx
  44dbf6:	89 51 3c             	mov    %edx,0x3c(%ecx)
  44dbf9:	8b 40 04             	mov    0x4(%eax),%eax
  44dbfc:	83 89 04 04 00 00 08 	orl    $0x8,0x404(%ecx)
  44dc03:	89 41 40             	mov    %eax,0x40(%ecx)
  44dc06:	8b f9                	mov    %ecx,%edi
  44dc08:	83 bf 98 01 00 00 00 	cmpl   $0x0,0x198(%edi)
  44dc0f:	74 2c                	je     0x44dc3d
  44dc11:	81 c7 54 01 00 00    	add    $0x154,%edi
  44dc17:	8d 5c 24 24          	lea    0x24(%esp),%ebx
  44dc1b:	e8 20 76 fb ff       	call   0x405240
  44dc20:	8b 10                	mov    (%eax),%edx
  44dc22:	8b 4d 08             	mov    0x8(%ebp),%ecx
  44dc25:	89 51 24             	mov    %edx,0x24(%ecx)
  44dc28:	8b 50 04             	mov    0x4(%eax),%edx
  44dc2b:	89 51 28             	mov    %edx,0x28(%ecx)
  44dc2e:	8b 40 08             	mov    0x8(%eax),%eax
  44dc31:	83 89 04 04 00 00 04 	orl    $0x4,0x404(%ecx)
  44dc38:	89 41 2c             	mov    %eax,0x2c(%ecx)
  44dc3b:	8b f9                	mov    %ecx,%edi
  44dc3d:	83 bf 20 02 00 00 00 	cmpl   $0x0,0x220(%edi)
  44dc44:	74 2d                	je     0x44dc73
  44dc46:	8d 87 dc 01 00 00    	lea    0x1dc(%edi),%eax
  44dc4c:	8d 5c 24 24          	lea    0x24(%esp),%ebx
  44dc50:	e8 5b 08 00 00       	call   0x44e4b0
  44dc55:	8a 4c 24 2c          	mov    0x2c(%esp),%cl
  44dc59:	8a 54 24 28          	mov    0x28(%esp),%dl
  44dc5d:	8a 44 24 24          	mov    0x24(%esp),%al
  44dc61:	88 8f 7a 03 00 00    	mov    %cl,0x37a(%edi)
  44dc67:	88 97 79 03 00 00    	mov    %dl,0x379(%edi)
  44dc6d:	88 87 78 03 00 00    	mov    %al,0x378(%edi)
  44dc73:	83 bf 4c 02 00 00 00 	cmpl   $0x0,0x24c(%edi)
  44dc7a:	74 16                	je     0x44dc92
  44dc7c:	81 c7 28 02 00 00    	add    $0x228,%edi
  44dc82:	e8 39 0b 00 00       	call   0x44e7c0
  44dc87:	8b 4d 08             	mov    0x8(%ebp),%ecx
  44dc8a:	88 81 7b 03 00 00    	mov    %al,0x37b(%ecx)
  44dc90:	8b f9                	mov    %ecx,%edi
  44dc92:	83 bf 78 02 00 00 00 	cmpl   $0x0,0x278(%edi)
  44dc99:	74 16                	je     0x44dcb1
  44dc9b:	81 c7 54 02 00 00    	add    $0x254,%edi
  44dca1:	e8 2a 0f 00 00       	call   0x44ebd0
  44dca6:	8b 55 08             	mov    0x8(%ebp),%edx
  44dca9:	d9 9a ac 02 00 00    	fstps  0x2ac(%edx)
  44dcaf:	8b fa                	mov    %edx,%edi
  44dcb1:	83 bf a4 02 00 00 00 	cmpl   $0x0,0x2a4(%edi)
  44dcb8:	74 16                	je     0x44dcd0
  44dcba:	81 c7 80 02 00 00    	add    $0x280,%edi
  44dcc0:	e8 0b 0f 00 00       	call   0x44ebd0
  44dcc5:	8b 45 08             	mov    0x8(%ebp),%eax
  44dcc8:	d9 98 b0 02 00 00    	fstps  0x2b0(%eax)
  44dcce:	8b f8                	mov    %eax,%edi
  44dcd0:	8b b7 04 04 00 00    	mov    0x404(%edi),%esi
  44dcd6:	8b c6                	mov    %esi,%eax
  44dcd8:	c1 e8 16             	shr    $0x16,%eax
  44dcdb:	83 e0 0f             	and    $0xf,%eax
  44dcde:	83 f8 09             	cmp    $0x9,%eax
  44dce1:	0f 85 62 02 00 00    	jne    0x44df49
  44dce7:	8b 87 b4 03 00 00    	mov    0x3b4(%edi),%eax
  44dced:	d9 47 2c             	flds   0x2c(%edi)
  44dcf0:	d9 5c 24 20          	fstps  0x20(%esp)
  44dcf4:	48                   	dec    %eax
  44dcf5:	89 44 24 1c          	mov    %eax,0x1c(%esp)
  44dcf9:	db 44 24 1c          	fildl  0x1c(%esp)
  44dcfd:	8b 9f 00 04 00 00    	mov    0x400(%edi),%ebx
  44dd03:	d9 5c 24 1c          	fstps  0x1c(%esp)
  44dd07:	d9 44 24 1c          	flds   0x1c(%esp)
  44dd0b:	d9 c0                	fld    %st(0)
  44dd0d:	dc 3d 78 7e 49 00    	fdivrl 0x497e78
  44dd13:	d9 5c 24 3c          	fstps  0x3c(%esp)
  44dd17:	d9 ee                	fldz
  44dd19:	d9 5c 24 18          	fstps  0x18(%esp)
  44dd1d:	da bf b8 03 00 00    	fidivrl 0x3b8(%edi)
  44dd23:	d9 5c 24 4c          	fstps  0x4c(%esp)
  44dd27:	f7 c6 00 80 00 00    	test   $0x8000,%esi
  44dd2d:	74 08                	je     0x44dd37
  44dd2f:	8b b7 78 03 00 00    	mov    0x378(%edi),%esi
  44dd35:	eb 06                	jmp    0x44dd3d
  44dd37:	8b b7 74 03 00 00    	mov    0x374(%edi),%esi
  44dd3d:	85 c0                	test   %eax,%eax
  44dd3f:	0f 8e c6 01 00 00    	jle    0x44df0b
  44dd45:	89 44 24 10          	mov    %eax,0x10(%esp)
  44dd49:	d9 e8                	fld1
  44dd4b:	89 73 10             	mov    %esi,0x10(%ebx)
  44dd4e:	d9 5b 0c             	fstps  0xc(%ebx)
  44dd51:	83 ec 08             	sub    $0x8,%esp
  44dd54:	d9 47 54             	flds   0x54(%edi)
  44dd57:	8b cb                	mov    %ebx,%ecx
  44dd59:	d8 47 70             	fadds  0x70(%edi)
  44dd5c:	d9 5b 14             	fstps  0x14(%ebx)
  44dd5f:	d9 44 24 20          	flds   0x20(%esp)
  44dd63:	d8 47 58             	fadds  0x58(%edi)
  44dd66:	d9 5b 18             	fstps  0x18(%ebx)
  44dd69:	d9 47 3c             	flds   0x3c(%edi)
  44dd6c:	dc 0d 60 7e 49 00    	fmull  0x497e60
  44dd72:	d8 47 40             	fadds  0x40(%edi)
  44dd75:	d9 5c 24 24          	fstps  0x24(%esp)
  44dd79:	d9 44 24 24          	flds   0x24(%esp)
  44dd7d:	d9 5c 24 04          	fstps  0x4(%esp)
  44dd81:	d9 44 24 28          	flds   0x28(%esp)
  44dd85:	d9 1c 24             	fstps  (%esp)
  44dd88:	e8 f3 11 00 00       	call   0x44ef80
  44dd8d:	d9 ee                	fldz
  44dd8f:	83 c3 1c             	add    $0x1c,%ebx
  44dd92:	d9 5b ec             	fstps  -0x14(%ebx)
  44dd95:	83 ec 08             	sub    $0x8,%esp
  44dd98:	d9 87 dc 03 00 00    	flds   0x3dc(%edi)
  44dd9e:	8b cb                	mov    %ebx,%ecx
  44dda0:	d8 87 f4 03 00 00    	fadds  0x3f4(%edi)
  44dda6:	d9 5c 24 2c          	fstps  0x2c(%esp)
  44ddaa:	d9 87 f8 03 00 00    	flds   0x3f8(%edi)
  44ddb0:	d8 87 e0 03 00 00    	fadds  0x3e0(%edi)
  44ddb6:	d9 5c 24 30          	fstps  0x30(%esp)
  44ddba:	d9 87 fc 03 00 00    	flds   0x3fc(%edi)
  44ddc0:	d8 87 e4 03 00 00    	fadds  0x3e4(%edi)
  44ddc6:	d9 5c 24 34          	fstps  0x34(%esp)
  44ddca:	d9 44 24 2c          	flds   0x2c(%esp)
  44ddce:	d8 87 e8 03 00 00    	fadds  0x3e8(%edi)
  44ddd4:	d9 5c 24 38          	fstps  0x38(%esp)
  44ddd8:	d9 87 ec 03 00 00    	flds   0x3ec(%edi)
  44ddde:	d8 44 24 30          	fadds  0x30(%esp)
  44dde2:	d9 5c 24 3c          	fstps  0x3c(%esp)
  44dde6:	d9 87 f0 03 00 00    	flds   0x3f0(%edi)
  44ddec:	d8 44 24 34          	fadds  0x34(%esp)
  44ddf0:	d9 5c 24 40          	fstps  0x40(%esp)
  44ddf4:	d9 43 e4             	flds   -0x1c(%ebx)
  44ddf7:	d8 44 24 38          	fadds  0x38(%esp)
  44ddfb:	d9 5b e4             	fstps  -0x1c(%ebx)
  44ddfe:	d9 44 24 3c          	flds   0x3c(%esp)
  44de02:	d8 43 e8             	fadds  -0x18(%ebx)
  44de05:	d9 5b e8             	fstps  -0x18(%ebx)
  44de08:	d9 44 24 40          	flds   0x40(%esp)
  44de0c:	dc 05 c0 7e 49 00    	faddl  0x497ec0
  44de12:	d9 5b ec             	fstps  -0x14(%ebx)
  44de15:	89 73 10             	mov    %esi,0x10(%ebx)
  44de18:	d9 e8                	fld1
  44de1a:	d9 5b 0c             	fstps  0xc(%ebx)
  44de1d:	d9 47 54             	flds   0x54(%edi)
  44de20:	d8 47 78             	fadds  0x78(%edi)
  44de23:	d9 5b 14             	fstps  0x14(%ebx)
  44de26:	d9 44 24 20          	flds   0x20(%esp)
  44de2a:	d8 47 58             	fadds  0x58(%edi)
  44de2d:	d9 5b 18             	fstps  0x18(%ebx)
  44de30:	d9 47 40             	flds   0x40(%edi)
  44de33:	d9 47 3c             	flds   0x3c(%edi)
  44de36:	dc 0d 60 7e 49 00    	fmull  0x497e60
  44de3c:	de e9                	fsubrp %st,%st(1)
  44de3e:	d9 5c 24 24          	fstps  0x24(%esp)
  44de42:	d9 44 24 24          	flds   0x24(%esp)
  44de46:	d9 5c 24 04          	fstps  0x4(%esp)
  44de4a:	d9 44 24 28          	flds   0x28(%esp)
  44de4e:	d9 1c 24             	fstps  (%esp)
  44de51:	e8 2a 11 00 00       	call   0x44ef80
  44de56:	d9 ee                	fldz
  44de58:	d9 5b 08             	fstps  0x8(%ebx)
  44de5b:	d9 87 f4 03 00 00    	flds   0x3f4(%edi)
  44de61:	d8 87 dc 03 00 00    	fadds  0x3dc(%edi)
  44de67:	d9 5c 24 50          	fstps  0x50(%esp)
  44de6b:	d9 87 f8 03 00 00    	flds   0x3f8(%edi)
  44de71:	d8 87 e0 03 00 00    	fadds  0x3e0(%edi)
  44de77:	d9 5c 24 54          	fstps  0x54(%esp)
  44de7b:	d9 87 fc 03 00 00    	flds   0x3fc(%edi)
  44de81:	d8 87 e4 03 00 00    	fadds  0x3e4(%edi)
  44de87:	d9 5c 24 58          	fstps  0x58(%esp)
  44de8b:	83 ec 08             	sub    $0x8,%esp
  44de8e:	d9 44 24 58          	flds   0x58(%esp)
  44de92:	83 c3 1c             	add    $0x1c,%ebx
  44de95:	d8 87 e8 03 00 00    	fadds  0x3e8(%edi)
  44de9b:	d9 5c 24 48          	fstps  0x48(%esp)
  44de9f:	d9 44 24 5c          	flds   0x5c(%esp)
  44dea3:	d8 87 ec 03 00 00    	fadds  0x3ec(%edi)
  44dea9:	d9 5c 24 4c          	fstps  0x4c(%esp)
  44dead:	d9 44 24 60          	flds   0x60(%esp)
  44deb1:	d8 87 f0 03 00 00    	fadds  0x3f0(%edi)
  44deb7:	d9 5c 24 50          	fstps  0x50(%esp)
  44debb:	d9 43 e4             	flds   -0x1c(%ebx)
  44debe:	d8 44 24 48          	fadds  0x48(%esp)
  44dec2:	d9 5b e4             	fstps  -0x1c(%ebx)
  44dec5:	d9 44 24 4c          	flds   0x4c(%esp)
  44dec9:	d8 43 e8             	fadds  -0x18(%ebx)
  44decc:	d9 5b e8             	fstps  -0x18(%ebx)
  44decf:	d9 44 24 50          	flds   0x50(%esp)
  44ded3:	dc 05 c0 7e 49 00    	faddl  0x497ec0
  44ded9:	d9 5b ec             	fstps  -0x14(%ebx)
  44dedc:	d9 44 24 54          	flds   0x54(%esp)
  44dee0:	d8 44 24 20          	fadds  0x20(%esp)
  44dee4:	d9 5c 24 20          	fstps  0x20(%esp)
  44dee8:	d9 44 24 44          	flds   0x44(%esp)
  44deec:	d9 5c 24 04          	fstps  0x4(%esp)
  44def0:	d9 44 24 28          	flds   0x28(%esp)
  44def4:	d9 1c 24             	fstps  (%esp)
  44def7:	e8 34 af 00 00       	call   0x458e30
  44defc:	83 6c 24 10 01       	subl   $0x1,0x10(%esp)
  44df01:	d9 5c 24 20          	fstps  0x20(%esp)
  44df05:	0f 85 3e fe ff ff    	jne    0x44dd49
  44df0b:	8b b7 00 04 00 00    	mov    0x400(%edi),%esi
  44df11:	d9 44 24 18          	flds   0x18(%esp)
  44df15:	8b 45 08             	mov    0x8(%ebp),%eax
  44df18:	d9 c0                	fld    %st(0)
  44df1a:	b9 07 00 00 00       	mov    $0x7,%ecx
  44df1f:	8b fb                	mov    %ebx,%edi
  44df21:	f3 a5                	rep movsl %ds:(%esi),%es:(%edi)
  44df23:	d8 40 58             	fadds  0x58(%eax)
  44df26:	d9 5b 18             	fstps  0x18(%ebx)
  44df29:	8b b0 00 04 00 00    	mov    0x400(%eax),%esi
  44df2f:	83 c6 1c             	add    $0x1c,%esi
  44df32:	8d 7b 1c             	lea    0x1c(%ebx),%edi
  44df35:	b9 07 00 00 00       	mov    $0x7,%ecx
  44df3a:	f3 a5                	rep movsl %ds:(%esi),%es:(%edi)
  44df3c:	d8 40 58             	fadds  0x58(%eax)
  44df3f:	d9 5b 34             	fstps  0x34(%ebx)
  44df42:	8b f8                	mov    %eax,%edi
  44df44:	e9 50 02 00 00       	jmp    0x44e199
  44df49:	83 f8 0d             	cmp    $0xd,%eax
  44df4c:	0f 85 47 02 00 00    	jne    0x44e199
  44df52:	d9 47 2c             	flds   0x2c(%edi)
  44df55:	8b 9f b4 03 00 00    	mov    0x3b4(%edi),%ebx
  44df5b:	d9 47 24             	flds   0x24(%edi)
  44df5e:	51                   	push   %ecx
  44df5f:	dc 0d 60 7e 49 00    	fmull  0x497e60
  44df65:	de e9                	fsubrp %st,%st(1)
  44df67:	d9 5c 24 20          	fstps  0x20(%esp)
  44df6b:	d9 44 24 20          	flds   0x20(%esp)
  44df6f:	d9 1c 24             	fstps  (%esp)
  44df72:	e8 59 af 00 00       	call   0x458ed0 ; normalizeAngle
  44df77:	d9 5c 24 20          	fstps  0x20(%esp)
  44df7b:	4b                   	dec    %ebx
  44df7c:	89 5c 24 1c          	mov    %ebx,0x1c(%esp)
  44df80:	db 44 24 1c          	fildl  0x1c(%esp)
  44df84:	8b 9f 00 04 00 00    	mov    0x400(%edi),%ebx
  44df8a:	d9 5c 24 1c          	fstps  0x1c(%esp)
  44df8e:	d9 47 24             	flds   0x24(%edi)
  44df91:	d9 44 24 1c          	flds   0x1c(%esp)
  44df95:	d9 c0                	fld    %st(0)
  44df97:	de fa                	fdivrp %st,%st(2)
  44df99:	d9 c9                	fxch   %st(1)
  44df9b:	d9 5c 24 3c          	fstps  0x3c(%esp)
  44df9f:	d9 ee                	fldz
  44dfa1:	d9 5c 24 18          	fstps  0x18(%esp)
  44dfa5:	da bf b8 03 00 00    	fidivrl 0x3b8(%edi)
  44dfab:	d9 5c 24 4c          	fstps  0x4c(%esp)
  44dfaf:	f7 c6 00 80 00 00    	test   $0x8000,%esi
  44dfb5:	74 08                	je     0x44dfbf
  44dfb7:	8b b7 78 03 00 00    	mov    0x378(%edi),%esi
  44dfbd:	eb 06                	jmp    0x44dfc5
  44dfbf:	8b b7 74 03 00 00    	mov    0x374(%edi),%esi
  44dfc5:	8b 87 b4 03 00 00    	mov    0x3b4(%edi),%eax
  44dfcb:	85 c0                	test   %eax,%eax
  44dfcd:	0f 8e c6 01 00 00    	jle    0x44e199
  44dfd3:	89 44 24 10          	mov    %eax,0x10(%esp)
  44dfd7:	d9 e8                	fld1
  44dfd9:	89 73 10             	mov    %esi,0x10(%ebx)
  44dfdc:	d9 5b 0c             	fstps  0xc(%ebx)
  44dfdf:	83 ec 08             	sub    $0x8,%esp
  44dfe2:	d9 47 54             	flds   0x54(%edi)
  44dfe5:	8b cb                	mov    %ebx,%ecx
  44dfe7:	d8 47 70             	fadds  0x70(%edi)
  44dfea:	d9 5b 14             	fstps  0x14(%ebx)
  44dfed:	d9 44 24 20          	flds   0x20(%esp)
  44dff1:	d8 47 58             	fadds  0x58(%edi)
  44dff4:	d9 5b 18             	fstps  0x18(%ebx)
  44dff7:	d9 47 3c             	flds   0x3c(%edi)
  44dffa:	dc 0d 60 7e 49 00    	fmull  0x497e60
  44e000:	d8 47 40             	fadds  0x40(%edi)
  44e003:	d9 5c 24 24          	fstps  0x24(%esp)
  44e007:	d9 44 24 24          	flds   0x24(%esp)
  44e00b:	d9 5c 24 04          	fstps  0x4(%esp)
  44e00f:	d9 44 24 28          	flds   0x28(%esp)
  44e013:	d9 1c 24             	fstps  (%esp)
  44e016:	e8 65 0f 00 00       	call   0x44ef80
  44e01b:	d9 ee                	fldz
  44e01d:	83 c3 1c             	add    $0x1c,%ebx
  44e020:	d9 5b ec             	fstps  -0x14(%ebx)
  44e023:	83 ec 08             	sub    $0x8,%esp
  44e026:	d9 87 f4 03 00 00    	flds   0x3f4(%edi)
  44e02c:	8b cb                	mov    %ebx,%ecx
  44e02e:	d8 87 dc 03 00 00    	fadds  0x3dc(%edi)
  44e034:	d9 5c 24 2c          	fstps  0x2c(%esp)
  44e038:	d9 87 f8 03 00 00    	flds   0x3f8(%edi)
  44e03e:	d8 87 e0 03 00 00    	fadds  0x3e0(%edi)
  44e044:	d9 5c 24 30          	fstps  0x30(%esp)
  44e048:	d9 87 fc 03 00 00    	flds   0x3fc(%edi)
  44e04e:	d8 87 e4 03 00 00    	fadds  0x3e4(%edi)
  44e054:	d9 5c 24 34          	fstps  0x34(%esp)
  44e058:	d9 87 e8 03 00 00    	flds   0x3e8(%edi)
  44e05e:	d8 44 24 2c          	fadds  0x2c(%esp)
  44e062:	d9 5c 24 38          	fstps  0x38(%esp)
  44e066:	d9 87 ec 03 00 00    	flds   0x3ec(%edi)
  44e06c:	d8 44 24 30          	fadds  0x30(%esp)
  44e070:	d9 5c 24 3c          	fstps  0x3c(%esp)
  44e074:	d9 87 f0 03 00 00    	flds   0x3f0(%edi)
  44e07a:	d8 44 24 34          	fadds  0x34(%esp)
  44e07e:	d9 5c 24 40          	fstps  0x40(%esp)
  44e082:	d9 44 24 38          	flds   0x38(%esp)
  44e086:	d8 43 e4             	fadds  -0x1c(%ebx)
  44e089:	d9 5b e4             	fstps  -0x1c(%ebx)
  44e08c:	d9 44 24 3c          	flds   0x3c(%esp)
  44e090:	d8 43 e8             	fadds  -0x18(%ebx)
  44e093:	d9 5b e8             	fstps  -0x18(%ebx)
  44e096:	d9 44 24 40          	flds   0x40(%esp)
  44e09a:	dc 05 c0 7e 49 00    	faddl  0x497ec0
  44e0a0:	d9 5b ec             	fstps  -0x14(%ebx)
  44e0a3:	89 73 10             	mov    %esi,0x10(%ebx)
  44e0a6:	d9 e8                	fld1
  44e0a8:	d9 5b 0c             	fstps  0xc(%ebx)
  44e0ab:	d9 47 54             	flds   0x54(%edi)
  44e0ae:	d8 47 78             	fadds  0x78(%edi)
  44e0b1:	d9 5b 14             	fstps  0x14(%ebx)
  44e0b4:	d9 44 24 20          	flds   0x20(%esp)
  44e0b8:	d8 47 58             	fadds  0x58(%edi)
  44e0bb:	d9 5b 18             	fstps  0x18(%ebx)
  44e0be:	d9 47 40             	flds   0x40(%edi)
  44e0c1:	d9 47 3c             	flds   0x3c(%edi)
  44e0c4:	dc 0d 60 7e 49 00    	fmull  0x497e60
  44e0ca:	de e9                	fsubrp %st,%st(1)
  44e0cc:	d9 5c 24 24          	fstps  0x24(%esp)
  44e0d0:	d9 44 24 24          	flds   0x24(%esp)
  44e0d4:	d9 5c 24 04          	fstps  0x4(%esp)
  44e0d8:	d9 44 24 28          	flds   0x28(%esp)
  44e0dc:	d9 1c 24             	fstps  (%esp)
  44e0df:	e8 9c 0e 00 00       	call   0x44ef80
  44e0e4:	d9 ee                	fldz
  44e0e6:	d9 5b 08             	fstps  0x8(%ebx)
  44e0e9:	d9 87 f4 03 00 00    	flds   0x3f4(%edi)
  44e0ef:	d8 87 dc 03 00 00    	fadds  0x3dc(%edi)
  44e0f5:	d9 5c 24 50          	fstps  0x50(%esp)
  44e0f9:	d9 87 f8 03 00 00    	flds   0x3f8(%edi)
  44e0ff:	d8 87 e0 03 00 00    	fadds  0x3e0(%edi)
  44e105:	d9 5c 24 54          	fstps  0x54(%esp)
  44e109:	d9 87 fc 03 00 00    	flds   0x3fc(%edi)
  44e10f:	d8 87 e4 03 00 00    	fadds  0x3e4(%edi)
  44e115:	d9 5c 24 58          	fstps  0x58(%esp)
  44e119:	83 ec 08             	sub    $0x8,%esp
  44e11c:	d9 44 24 58          	flds   0x58(%esp)
  44e120:	83 c3 1c             	add    $0x1c,%ebx
  44e123:	d8 87 e8 03 00 00    	fadds  0x3e8(%edi)
  44e129:	d9 5c 24 48          	fstps  0x48(%esp)
  44e12d:	d9 44 24 5c          	flds   0x5c(%esp)
  44e131:	d8 87 ec 03 00 00    	fadds  0x3ec(%edi)
  44e137:	d9 5c 24 4c          	fstps  0x4c(%esp)
  44e13b:	d9 44 24 60          	flds   0x60(%esp)
  44e13f:	d8 87 f0 03 00 00    	fadds  0x3f0(%edi)
  44e145:	d9 5c 24 50          	fstps  0x50(%esp)
  44e149:	d9 44 24 48          	flds   0x48(%esp)
  44e14d:	d8 43 e4             	fadds  -0x1c(%ebx)
  44e150:	d9 5b e4             	fstps  -0x1c(%ebx)
  44e153:	d9 44 24 4c          	flds   0x4c(%esp)
  44e157:	d8 43 e8             	fadds  -0x18(%ebx)
  44e15a:	d9 5b e8             	fstps  -0x18(%ebx)
  44e15d:	d9 44 24 50          	flds   0x50(%esp)
  44e161:	dc 05 c0 7e 49 00    	faddl  0x497ec0
  44e167:	d9 5b ec             	fstps  -0x14(%ebx)
  44e16a:	d9 44 24 54          	flds   0x54(%esp)
  44e16e:	d8 44 24 20          	fadds  0x20(%esp)
  44e172:	d9 5c 24 20          	fstps  0x20(%esp)
  44e176:	d9 44 24 44          	flds   0x44(%esp)
  44e17a:	d9 5c 24 04          	fstps  0x4(%esp)
  44e17e:	d9 44 24 28          	flds   0x28(%esp)
  44e182:	d9 1c 24             	fstps  (%esp)
  44e185:	e8 a6 ac 00 00       	call   0x458e30
  44e18a:	83 6c 24 10 01       	subl   $0x1,0x10(%esp)
  44e18f:	d9 5c 24 20          	fstps  0x20(%esp)
  44e193:	0f 85 3e fe ff ff    	jne    0x44dfd7
  44e199:	8b 87 28 04 00 00    	mov    0x428(%edi),%eax
  44e19f:	85 c0                	test   %eax,%eax
  44e1a1:	74 0c                	je     0x44e1af
  44e1a3:	8b cf                	mov    %edi,%ecx
  44e1a5:	ff d0                	call   *%eax
  44e1a7:	85 c0                	test   %eax,%eax
  44e1a9:	0f 85 15 d8 ff ff    	jne    0x44b9c4
  44e1af:	8b 57 60             	mov    0x60(%edi),%edx
  44e1b2:	8b 4f 68             	mov    0x68(%edi),%ecx
  44e1b5:	89 57 5c             	mov    %edx,0x5c(%edi)
  44e1b8:	d9 01                	flds   (%ecx)
  44e1ba:	dc 1d 28 7f 49 00    	fcompl 0x497f28
  44e1c0:	df e0                	fnstsw %ax
  44e1c2:	f6 c4 41             	test   $0x41,%ah
  44e1c5:	75 34                	jne    0x44e1fb
  44e1c7:	d9 05 24 7f 49 00    	flds   0x497f24
  44e1cd:	d8 19                	fcomps (%ecx)
  44e1cf:	df e0                	fnstsw %ax
  44e1d1:	f6 c4 41             	test   $0x41,%ah
  44e1d4:	75 25                	jne    0x44e1fb
  44e1d6:	d9 47 64             	flds   0x64(%edi)
  44e1d9:	42                   	inc    %edx
  44e1da:	dc 05 48 7e 49 00    	faddl  0x497e48
  44e1e0:	89 57 60             	mov    %edx,0x60(%edi)
  44e1e3:	33 c0                	xor    %eax,%eax
  44e1e5:	d9 5f 64             	fstps  0x64(%edi)
  44e1e8:	d9 44 24 74          	flds   0x74(%esp)
  44e1ec:	d9 1d 48 79 4a 00    	fstps  0x4a7948
  44e1f2:	5f                   	pop    %edi
  44e1f3:	5e                   	pop    %esi
  44e1f4:	5b                   	pop    %ebx
  44e1f5:	8b e5                	mov    %ebp,%esp
  44e1f7:	5d                   	pop    %ebp
  44e1f8:	c2 04 00             	ret    $0x4
  44e1fb:	d9 01                	flds   (%ecx)
  44e1fd:	d8 47 64             	fadds  0x64(%edi)
  44e200:	d9 5c 24 1c          	fstps  0x1c(%esp)
  44e204:	d9 44 24 1c          	flds   0x1c(%esp)
  44e208:	d9 57 64             	fsts   0x64(%edi)
  44e20b:	e8 d0 82 03 00       	call   0x4864e0
  44e210:	d9 44 24 74          	flds   0x74(%esp)
  44e214:	89 47 60             	mov    %eax,0x60(%edi)
  44e217:	d9 1d 48 79 4a 00    	fstps  0x4a7948
  44e21d:	5f                   	pop    %edi
  44e21e:	5e                   	pop    %esi
  44e21f:	33 c0                	xor    %eax,%eax
  44e221:	5b                   	pop    %ebx
  44e222:	8b e5                	mov    %ebp,%esp
  44e224:	5d                   	pop    %ebp
  44e225:	c2 04 00             	ret    $0x4
