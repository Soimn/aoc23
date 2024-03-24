#define SMN_IMPLEMENTATION
#include "../smn.h"

enum Direction
{
  Dir_N = 0,
  //Dir_NE,
  Dir_E,
  //Dir_SE,
  Dir_S,
  //Dir_SW,
  Dir_W,
  //Dir_NW,
  DIRECTION_COUNT
};

#define OPPOSITE_DIR(D) (((D) + DIRECTION_COUNT/2) % DIRECTION_COUNT)
#define X_STEP(D) (((D) == Dir_E) - ((D) == Dir_W))
#define Y_STEP(D) (((D) == Dir_N) - ((D) == Dir_S))

typedef struct Node_Pred
{
  u32 dist;
  u32 prev_node;
  u8 pred_idx;
  u8 dir;
  u8 ttl;
} Node_Pred;

typedef struct Node
{
  s8 path_dir;
  u8 weight;
  s8 dir;
  s8 alt_dir;
  Node_Pred preds[4];
} Node;

typedef struct Edge
{
  u32 key;
  u32 start_node;
  u32 end_node;
  u8 pred_idx;
  u8 dir;
  u8 ttl;
} Edge;

typedef struct Edge_Queue
{
  SB(Edge) backing;
  u32 heap_size;
} Edge_Queue;

bool
EdgeQueue_IsEmpty(Edge_Queue* queue)
{
  return (queue->heap_size == 0);
}

void
EdgeQueue_Enqueue(Edge_Queue* queue, Edge edge)
{
  SB_Append(&queue->backing, edge);
  queue->heap_size += 1;

  uint idx = queue->heap_size-1;

  uint parent_idx = idx/2;
  while (queue->backing[idx].key < queue->backing[parent_idx].key)
  {
    Edge tmp = queue->backing[idx];
    queue->backing[idx]        = queue->backing[parent_idx];
    queue->backing[parent_idx] = tmp;

    idx        = parent_idx;
    parent_idx = idx/2;
  }
}

Edge
EdgeQueue_Dequeue(Edge_Queue* queue)
{
  ASSERT(queue->heap_size > 0);

  Edge dequeued_entry = queue->backing[0];

  queue->backing[0] = queue->backing[SB_Len(&queue->backing)-1];
  SB_RemoveLast(&queue->backing);
  queue->heap_size -= 1;

  for (uint smallest_idx = 0;;)
  {
    uint idx       = smallest_idx;
    uint left_idx  = 2*smallest_idx;
    uint right_idx = 2*smallest_idx + 1;

    if (left_idx < queue->heap_size && queue->backing[left_idx].key < queue->backing[smallest_idx].key)
    {
      smallest_idx = left_idx;
    }

    if (right_idx < queue->heap_size && queue->backing[right_idx].key < queue->backing[smallest_idx].key)
    {
      smallest_idx = right_idx;
    }

    if (smallest_idx == idx) break;
    else
    {
      Edge tmp = queue->backing[idx];
      queue->backing[idx]          = queue->backing[smallest_idx];
      queue->backing[smallest_idx] = tmp;
      continue;
    }
  }

  return dequeued_entry;
}

int
main(int argc, char** argv)
{
  ASSERT(argc == 2);

  String input = {0};
  {
    FILE* file;
    errno_t open_err = fopen_s(&file, argv[1], "rb");
    ASSERT(open_err == 0);

    fseek(file, 0, SEEK_END);
    umm file_size = ftell(file);
    rewind(file);

    input.data = malloc(file_size);
    input.size = file_size;

    umm bytes_read = fread(input.data, 1, file_size, file);

    ASSERT(bytes_read == file_size);

    fclose(file);
  }

  uint size = 0;
  while (size < input.size && input.data[size] != '\r') ++size;
  ASSERT(size > 0 && input.size/(size+2) == size);

  Node* nodes = malloc(size*size*sizeof(Node));

  for (uint j = 0; j < size; ++j)
  {
    for (uint i = 0; i < size; ++i)
    {
      nodes[j*size + i] = (Node){
        .path_dir = -1,
        .weight   = input.data[j*(size+2) + i]&0xF,
        .dir      = -1,
        .alt_dir  = -1,
        .preds    = {
          [0] = { .dist = U32_MAX },
          [1] = { .dist = U32_MAX },
          [2] = { .dist = U32_MAX },
          [3] = { .dist = U32_MAX },
        },
      };
    }
  }

  Edge_Queue queue = {0};

  EdgeQueue_Enqueue(&queue, (Edge){
      .key        = nodes[1].weight,
      .start_node = 0,
      .end_node   = 1,
      .dir        = Dir_E,
      .ttl        = 2
  });
  EdgeQueue_Enqueue(&queue, (Edge){
      .key        = nodes[size].weight,
      .start_node = 0,
      .end_node   = size,
      .dir        = Dir_N,
      .ttl        = 2
  });

  u32 goal_node = size*size-1;
  s8 goal_ttl   = -1;
  while (nodes[goal_node].dir == -1)
  {
    Edge edge = EdgeQueue_Dequeue(&queue);

    if (nodes[edge.end_node].dir == -1                                             ||
        edge.dir != nodes[edge.end_node].dir && nodes[edge.end_node].alt_dir == -1 ||
        edge.dir == nodes[edge.end_node].dir && (nodes[edge.end_node].preds[edge.ttl].dist == U32_MAX &&
                                                edge.key < nodes[edge.end_node].preds[edge.ttl+1].dist))
    {
      u8 insert_idx;
      if (nodes[edge.end_node].dir == -1 || nodes[edge.end_node].dir == edge.dir)
      {
        nodes[edge.end_node].dir = edge.dir;
        insert_idx = edge.ttl;
      }
      else
      {
        nodes[edge.end_node].alt_dir = edge.dir;
        insert_idx = 3;
      }

      nodes[edge.end_node].preds[insert_idx] = (Node_Pred){
        .dist      = edge.key,
        .prev_node = edge.start_node,
        .pred_idx  = edge.pred_idx,
        .dir       = edge.dir,
        .ttl       = edge.ttl,
      };

      for (u8 i = 0; i < DIRECTION_COUNT; ++i)
      {
        s8 dx  = X_STEP(i);
        s8 dy  = Y_STEP(i);
        s8 ttl = (s8)(i == edge.dir ? edge.ttl-1 : 2);

        if (ttl >= 0                     &&
            i != OPPOSITE_DIR(edge.dir) &&
            !(edge.end_node % size == 0      && dx == -1 ||
              edge.end_node % size == size-1 && dx ==  1 ||
              edge.end_node / size == 0      && dy == -1 ||
              edge.end_node / size == size-1 && dy ==  1))
        {
          u32 end_node = edge.end_node + dy*size + dx;
          u32 dist     = edge.key + nodes[end_node].weight;
          
          if (end_node == goal_node)
          {
            nodes[end_node].dir = i;
            nodes[end_node].preds[ttl] = (Node_Pred){
              .dist      = dist,
              .prev_node = edge.end_node,
              .pred_idx  = insert_idx,
              .dir       = i,
              .ttl       = ttl,
            };

            goal_ttl = ttl;

            break;
          }
          else if (nodes[end_node].dir == -1                            ||
              i != nodes[end_node].dir && nodes[end_node].alt_dir == -1 ||
              i == nodes[end_node].dir && nodes[end_node].preds[ttl].dist == U32_MAX)
          {
            EdgeQueue_Enqueue(&queue, (Edge){
                .key        = dist,
                .start_node = edge.end_node,
                .end_node   = end_node,
                .pred_idx   = insert_idx,
                .dir        = i,
                .ttl        = ttl,
            });
          }
        }
      }
    }
  }

  for (u32 node = goal_node, pred = goal_ttl; node != 0;)
  {
    nodes[node].path_dir = nodes[node].preds[pred].dir;

    u32 prev_node = nodes[node].preds[pred].prev_node;
    u32 prev_pred = nodes[node].preds[pred].pred_idx;

    node = prev_node;
    pred = prev_pred;
  }

  for (uint j = 0; j < size; ++j)
  {
    for (uint i = 0; i < size; ++i)
    {
      Node* node = &nodes[j*size + i];
      putchar(node->path_dir == -1 ? '0'+node->weight : (u8[4]){'v', '>', '^', '<'}[node->path_dir]);
    }
    putchar('\n');
  }

  printf("Part 1: %u\n", nodes[goal_node].preds[goal_ttl].dist);

  return 0;
}
