{
  'targets': [
    {
      'target_name': 'ktest',
      'type': 'executable',
      'include_dirs': [
        '../../include',
      ],
      'cflags':['-DAVRPP_KTEST','-O0'],      
      'sources': [
        'test.cpp',
      ],
    },
  ],
}
